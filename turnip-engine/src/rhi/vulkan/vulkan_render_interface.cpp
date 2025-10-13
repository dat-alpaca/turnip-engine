#include "vulkan_render_interface.hpp"
#include "rhi/vulkan/allocators/allocators.hpp"
#include "rhi/vulkan/initializers/initializers.hpp"

namespace tur::vulkan
{
	void RenderInterfaceVulkan::initialize(const ConfigData& configData, Window& window)
	{
		const auto& vulkanConfig = configData.vulkanConfig;
		rWindow = &window;

		mDeletionQueue.initialize(this);

		// Instance:
		initialize_instance(mState, configData);

		// Surface:
		mState.surface = platform::WindowingSystem::get_binder().get_vulkan_surface(mState.instance);

		// Physical device:
		initialize_physical_device(mState, vulkanConfig.physicalDeviceRequirements);

		// Logical device:
		initialize_logical_device(mState, vulkanConfig);
		mResources.initialize(this);

		// VMA Allocator:
		initialize_vma_allocator(mState);

		// Swapchain:
		initialize_swapchain(mState, {});

		// Command Pool:
		initialize_command_pool(mState);

		// Immediate Commands:
		{
			mImmCommandPool = initialize_command_pool(mState.queueList, mState.logicalDevice);
			mImmCommandBuffer = allocate_single_primary_command_buffer(mState.logicalDevice, mImmCommandPool);
			mImmFence = allocate_signaled_fence(mState.logicalDevice);
		}

		// Frame Data:
		mFrameDataHolder.initialize(mState.logicalDevice, mState.commandPool, mState.swapchainTextures.size());

		// Main Render Target:
		create_main_render_target();
	}
	void RenderInterfaceVulkan::shutdown()
	{
		auto& device = mState.logicalDevice;
		device.waitIdle();

		destroy_swapchain(mState);

		// Immediate Commands:
		{
			mState.logicalDevice.destroyCommandPool(mImmCommandPool);
			mState.logicalDevice.destroyFence(mImmFence);
		}

		// Resources:
		{
			mResources.destroy();
			mDeletionQueue.flush();

			mFrameDataHolder.shutdown(mState.logicalDevice);
			mState.logicalDevice.destroyCommandPool(mState.commandPool);
		}

		destroy_vma_allocator(mState.vmaAllocator);

		device.destroy();
		destroy_instance_messenger(mState);

		platform::WindowingSystem::get_binder().destroy_vulkan_surface(mState.instance, mState.surface);
		mState.instance.destroy();
	}
	void RenderInterfaceVulkan::on_event(Event& event)
	{
		Subscriber subscriber(event);
		subscriber.subscribe<WindowFramebufferEvent>(
			[&](const WindowFramebufferEvent&) -> bool
			{
				mWindowResized = true;
				return false;
			}
		);
	}

	CommandBuffer RenderInterfaceVulkan::create_command_buffer()
	{
		return CommandBuffer(this);
	}
	vk::CommandBuffer RenderInterfaceVulkan::get_current_command_buffer()
	{
		return mFrameDataHolder.get_current_frame_data().commandBuffer;
	}

	queue_handle RenderInterfaceVulkan::get_queue(QueueOperation operation)
	{
		queue_handle handle = mState.queueList.get_queue_index(operation);

		if (handle == invalid_handle)
			TUR_LOG_ERROR("Failed to get {} queue", get_queue_operation_strings(operation)[0]);

		return handle;
	}

	void RenderInterfaceVulkan::create_main_render_target()
	{
		if (mState.mainRenderTargetHandle != invalid_handle)
			mResources.destroy_render_target(mState.mainRenderTargetHandle);

		RenderTargetDescriptor descriptor;
		{
			TextureDescriptor textureDesc;
			textureDesc.format = TextureFormat::RGBA8_UNORM; // TODO: add a global variable to it
			textureDesc.width = mState.swapchainExtent.width;
			textureDesc.height = mState.swapchainExtent.height;

			AttachmentDescription attachment;
			attachment.textureDescriptor = textureDesc;

			descriptor.attachmentDescriptions.push_back(attachment);
		}

		mState.mainRenderTargetHandle = mResources.create_render_target(descriptor);
		mState.mainRenderTarget = mResources.get_render_targets().get(mState.mainRenderTargetHandle);
	}

	void RenderInterfaceVulkan::begin_frame()
	{
		auto& device = mState.logicalDevice;
		auto& swapchain = mState.swapchain;

		mFrameDataHolder.advance();
		auto& frameData = mFrameDataHolder.get_current_frame_data();

		try
		{
			auto result = device.waitForFences({frameData.recordingFence}, true, RecordingFenceTimeout);
		}
		catch (vk::SystemError& err)
		{
			TUR_LOG_CRITICAL("Failed to wait for fences. {}", err.what());
		}

		mDeletionQueue.flush();

		// Acquiring swapchain:
		{
			auto imageResult =
				device.acquireNextImageKHR(swapchain, ImageAvailableTimeout, frameData.imageAvailableSemaphore);
			mFrameDataHolder.set_image_buffer_index(imageResult.value);

			if (imageResult.result == vk::Result::eErrorOutOfDateKHR
				|| imageResult.result == vk::Result::eSuboptimalKHR)
				utils::recreate_swapchain(*this);

			else if (imageResult.result != vk::Result::eSuccess)
			{
				TUR_LOG_CRITICAL("Failed to acquire swapchain image");
			}
		}

		// Begin:
		device.resetFences({frameData.recordingFence});

		vk::CommandBufferBeginInfo beginInfo = {};
		{
			beginInfo.pInheritanceInfo = nullptr;
			beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
		}

		frameData.commandBuffer.reset();

		try
		{
			frameData.commandBuffer.begin(beginInfo);
		}
		catch (vk::SystemError& err)
		{
			TUR_LOG_ERROR("Failed to begin() recording to vulkan command buffer.", err.what());
		}
	}
	void RenderInterfaceVulkan::submit(queue_handle graphicsQueue)
	{
		// Submits data from the currently bound framedata object

		vk::Queue queue = mState.queueList.get_queue(graphicsQueue).queue;
		auto& frameData = mFrameDataHolder.get_current_frame_data();

		vk::Semaphore waitSemaphores[] = {frameData.imageAvailableSemaphore};
		vk::Semaphore signalSemaphores[] = {frameData.renderFinishedSemaphore};
		vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
		vk::SubmitInfo submitInfo = {};
		{
			submitInfo.pWaitDstStageMask = waitStages;

			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = waitSemaphores;

			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = signalSemaphores;

			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &frameData.commandBuffer;
		}

		try
		{
			queue.submit(submitInfo, frameData.recordingFence);
		}
		catch (vk::SystemError& err)
		{
			TUR_LOG_ERROR("Failed to submit commands to the graphics queue. {}", err.what());
		}
	}
	void RenderInterfaceVulkan::present(queue_handle presentQueue)
	{
		// Submits a present command using "presentQueue" and the internal swapchain

		vk::Queue queue = mState.queueList.get_queue(presentQueue).queue;
		auto& frameHolder = mFrameDataHolder;
		const auto& frameData = frameHolder.get_current_frame_data();

		vk::Semaphore waitSemaphores[] = {frameData.renderFinishedSemaphore};

		u32 imageIndices[] = {frameHolder.get_image_buffer_index()};
		vk::PresentInfoKHR presentInfo = {};
		{
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = waitSemaphores;
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = &mState.swapchain;
			presentInfo.pImageIndices = imageIndices;
		}

		vk::Result presentResult = {};
		try
		{
			presentResult = queue.presentKHR(presentInfo);
		}
		catch (vk::OutOfDateKHRError& outOfDate)
		{
			utils::recreate_swapchain(*this);
		}
		catch (vk::SystemError& err)
		{
			if (presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR
				|| mWindowResized)
			{
				mWindowResized = false;
				utils::recreate_swapchain(*this);
			}
			else if (presentResult != vk::Result::eSuccess)
				TUR_LOG_CRITICAL("Failed to recreate swapchain image. {}", err.what());
		}
	}
	void RenderInterfaceVulkan::end_frame()
	{
		auto& currentCommandBuffer = mFrameDataHolder.get_current_frame_data().commandBuffer;

		try
		{
			currentCommandBuffer.end();
		}
		catch (vk::SystemError err)
		{
			throw std::runtime_error("Failed to end() recording to vulkan command buffer.");
		}
	}
}

namespace tur::vulkan::utils
{
	void submit_immediate_command(RenderInterfaceVulkan& rhi, immediate_cmd_function&& command)
	{
		auto& state = rhi.get_state();
		auto& fence = rhi.mImmFence;
		auto& commandBuffer = rhi.mImmCommandBuffer;

		state.logicalDevice.resetFences(fence);
		commandBuffer.reset();

		vk::CommandBufferBeginInfo beginInfo = {};
		beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

		commandBuffer.begin(beginInfo);
		command(commandBuffer);
		commandBuffer.end();

		vk::CommandBufferSubmitInfo submitCommandInfo = {};
		{
			submitCommandInfo.commandBuffer = commandBuffer;
			submitCommandInfo.deviceMask = 0;
		}

		vk::SubmitInfo2 submitInfo = {};
		{
			submitInfo.signalSemaphoreInfoCount = 0;
			submitInfo.pSignalSemaphoreInfos = nullptr;

			submitInfo.waitSemaphoreInfoCount = 0;
			submitInfo.pWaitSemaphoreInfos = nullptr;

			submitInfo.commandBufferInfoCount = 1;
			submitInfo.pCommandBufferInfos = &submitCommandInfo;
		}

		auto queueOpt = state.queueList.get_queue(QueueOperation::TRANSFER);
		if (!queueOpt.has_value())
			return TUR_LOG_ERROR("Invalid immediate command transfer queue fetched.");

		auto& queue = queueOpt.value().queue;

		try
		{
			queue.submit2(submitInfo, fence);
		}
		catch (vk::DeviceLostError& err)
		{
			TUR_LOG_CRITICAL("Failed to submit immediate command: {}", err.what());
		}
		catch (vk::SystemError& err)
		{
			TUR_LOG_CRITICAL("Failed to submit immediate command: {}", err.what());
		}

		try
		{
			auto _ = state.logicalDevice.waitForFences(fence, true, rhi.RecordingFenceTimeout);
		}
		catch (vk::SystemError& err)
		{
			TUR_LOG_CRITICAL("Failed to wait for queue: {}", err.what());
		}
	}

	void recreate_swapchain(RenderInterfaceVulkan& rhi)
	{
		auto& state = rhi.get_state();
		auto* window = rhi.get_window();

		glm::vec2 size = window->get_dimensions();
		while (size.x == 0 || size.y == 0)
		{
			size = window->get_dimensions();
			window->wait_events();
		}

		state.logicalDevice.waitIdle();
		destroy_swapchain(state);

		SwapchainRequirements requirements;
		{
			requirements.extent = Extent2D{static_cast<u32>(size.x), static_cast<u32>(size.y)};
			initialize_swapchain(state, requirements);
		}

		rhi.mFrameDataHolder.initialize(state.logicalDevice, state.commandPool, state.swapchainTextures.size());
		rhi.create_main_render_target();
	}

	void transition_texture_layout(
		RenderInterfaceVulkan& rhi, Texture& texture, const ImageTransitionDescription& description
	)
	{
		auto& commandBuffer = rhi.get_frame_data().get_current_frame_data().commandBuffer;

		if (texture.layout == description.newLayout)
		{
			TUR_LOG_WARN("Attempted to transition to the same image layout.");
			return;
		}

		vk::ImageAspectFlags aspectMask;
		vk::ImageSubresourceRange subresourceRange;

		vk::ImageMemoryBarrier2 imageBarrier = {};
		{
			imageBarrier.srcStageMask = description.srcStageMask;
			imageBarrier.srcAccessMask = description.srcAccessMask;

			imageBarrier.dstStageMask = description.dstStageMask;
			imageBarrier.dstAccessMask = description.dstAccessMask;

			imageBarrier.oldLayout = texture.layout;
			imageBarrier.newLayout = description.newLayout;

			if (description.newLayout == vk::ImageLayout::eDepthAttachmentOptimal)
				aspectMask = vk::ImageAspectFlagBits::eDepth;
			else
				aspectMask = vk::ImageAspectFlagBits::eColor;

			subresourceRange.aspectMask = aspectMask;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = texture.descriptor.mipLevels;
			subresourceRange.baseArrayLayer = 0;
			subresourceRange.layerCount = 1; // TODO: change to texture.layers

			imageBarrier.subresourceRange = subresourceRange;
			imageBarrier.image = texture.image;
		}

		vk::DependencyInfo dependencyInfo = vk::DependencyInfo().setImageMemoryBarriers(imageBarrier);

		try
		{
			commandBuffer.pipelineBarrier2(dependencyInfo);
		}
		catch (vk::SystemError& err)
		{
			TUR_LOG_CRITICAL("Failed to issue transition image layout command. {}", err.what());
		}

		texture.layout = description.newLayout;
	}
}
