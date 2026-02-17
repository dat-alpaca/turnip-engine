#include "vulkan_render_interface.hpp"
#include "rhi/vulkan/allocators/allocators.hpp"
#include "rhi/vulkan/initializers/initializers.hpp"
#include "rhi/vulkan/objects/texture.hpp"

// TODO: substitute vk::result checks

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
		{
			glm::vec2 size = rWindow->get_dimensions();

			SwapchainRequirements requirements = {};
			requirements.extent = Extent2D{.width=static_cast<u32>(size.x), .height=static_cast<u32>(size.y)};
		
			initialize_swapchain(mState, requirements);
		}
		

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
		vk::Result result = device.waitIdle();
		if (result != vk::Result::eSuccess)
			TUR_LOG_CRITICAL("Failed to wait for device idle");

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

		// TODO: global render target settings
		// TODO: different load/store ops

		RenderTargetDescriptor descriptor;
		{
			TextureDescriptor textureDesc;
			textureDesc.format = TextureFormat::RGBA8_UNORM; 
			textureDesc.width = mState.swapchainExtent.width;
			textureDesc.height = mState.swapchainExtent.height;
			textureDesc.isColorAttachment = true;

			descriptor.colorAttachment.textureDescriptor = textureDesc;
		}

		{
			TextureDescriptor depthDesc;
			depthDesc.format = TextureFormat::DEPTH_STENCIL24_S8U_INT;
			depthDesc.width = mState.swapchainExtent.width;
			depthDesc.height = mState.swapchainExtent.height;
			depthDesc.isDepthStencilAttachment = true;

			descriptor.depthStencilAttachment.textureDescriptor = depthDesc;
		}

		mState.mainRenderTargetHandle = mResources.create_render_target(descriptor);
		mState.mainRenderTarget = mResources.get_render_targets().get(mState.mainRenderTargetHandle);
	}

	bool RenderInterfaceVulkan::begin_frame()
	{
		auto& device = mState.logicalDevice;
		auto& swapchain = mState.swapchain;

		mFrameDataHolder.advance();
		auto& frameData = mFrameDataHolder.get_current_frame_data();

		vk::Result result = device.waitForFences({frameData.recordingFence}, true, RecordingFenceTimeout);
		if (result != vk::Result::eSuccess)
			TUR_LOG_CRITICAL("Failed to wait for fences.");

		mDeletionQueue.flush();

		// Acquiring swapchain:
		{
			u32 imageIndex = 0;
			VkResult result = vkAcquireNextImageKHR(
				device, swapchain, ImageAvailableTimeout, frameData.imageReadySemaphore, nullptr, &imageIndex
			);
			mFrameDataHolder.set_image_buffer_index(imageIndex);

			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			{
				utils::recreate_swapchain(*this);
				return false;
			}

			else if (result == VK_SUCCESS || result == VK_NOT_READY || result == VK_TIMEOUT)
			{
			}

			else
				TUR_LOG_CRITICAL("Failed to acquire swapchain image");
		}

		// Reset main command buffer:
		auto _ = device.resetFences({frameData.recordingFence});
		auto __ = frameData.commandBuffer.reset();

		return true;
	}
	void RenderInterfaceVulkan::submit(queue_handle graphicsQueue)
	{
		// Submits data from the currently bound framedata object

		vk::Queue queue = mState.queueList.get_queue(graphicsQueue).queue;
		auto& frameData = mFrameDataHolder.get_current_frame_data();

		vk::Semaphore waitSemaphores[] = {frameData.imageReadySemaphore};
		vk::Semaphore signalSemaphores[] = {mFrameDataHolder.get_render_finished_semaphore()};
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

		vk::Result result = queue.submit(submitInfo, frameData.recordingFence);
		if (result != vk::Result::eSuccess)
			TUR_LOG_ERROR("Failed to submit commands to the graphics queue.");
	}
	void RenderInterfaceVulkan::present(queue_handle presentQueue)
	{
		// Submits a present command using "presentQueue" and the internal swapchain

		vk::Queue queue = mState.queueList.get_queue(presentQueue).queue;
		auto& frameHolder = mFrameDataHolder;
		const auto& frameData = frameHolder.get_current_frame_data();

		vk::Semaphore waitSemaphores[] = {mFrameDataHolder.get_render_finished_semaphore()};

		u32 imageIndices[] = {frameHolder.get_image_buffer_index()};

		vk::PresentInfoKHR presentInfo = {};
		{
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = waitSemaphores;
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = &mState.swapchain;
			presentInfo.pImageIndices = imageIndices;
		}

		VkResult result = vkQueuePresentKHR(queue, reinterpret_cast<const VkPresentInfoKHR*>(&presentInfo));
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mWindowResized)
		{
			mWindowResized = false;
			utils::recreate_swapchain(*this);
		}
		else if (result != VK_SUCCESS)
			TUR_LOG_CRITICAL("Failed to recreate swapchain image. {}", static_cast<i32>(result));
	}
	void RenderInterfaceVulkan::end_frame()
	{
		/* Blank */
	}
}

namespace tur::vulkan::utils
{
	void submit_immediate_command(RenderInterfaceVulkan& rhi, immediate_cmd_function&& command)
	{
		auto& state = rhi.get_state();
		auto& fence = rhi.mImmFence;
		auto& commandBuffer = rhi.mImmCommandBuffer;

		auto _ = state.logicalDevice.resetFences(fence);
		auto __ = commandBuffer.reset();

		vk::CommandBufferBeginInfo beginInfo = {};
		beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

		vk::Result result = commandBuffer.begin(beginInfo);
		if (result != vk::Result::eSuccess)
			TUR_LOG_WARN("Failed to begin() immediate command buffer.");

		command(commandBuffer);

		result = commandBuffer.end();
		if (result != vk::Result::eSuccess)
			TUR_LOG_WARN("Failed to end() immediate command buffer.");

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

		result = queue.submit2(submitInfo, fence);
		if (result != vk::Result::eSuccess)
			TUR_LOG_CRITICAL("Failed to submit immediate command.");

		result = state.logicalDevice.waitForFences(fence, true, rhi.RecordingFenceTimeout);
		if (result != vk::Result::eSuccess)
			TUR_LOG_CRITICAL("Failed to wait for queue.");
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

		vk::Result result = state.logicalDevice.waitIdle();
		if (result != vk::Result::eSuccess)
			TUR_LOG_CRITICAL("Failed to wait device idle");

		destroy_swapchain(state);

		SwapchainRequirements requirements;
		{
			requirements.extent = Extent2D{ .width = static_cast<u32>(size.x), .height = static_cast<u32>(size.y)};
			initialize_swapchain(state, requirements);
		}

		rhi.mFrameDataHolder.initialize(state.logicalDevice, state.commandPool, state.swapchainTextures.size());
		rhi.create_main_render_target();
	}

	void transition_texture_layout(RenderInterfaceVulkan& rhi, Texture& texture, const ImageTransitionDescription& description)
	{
		auto& commandBuffer = rhi.get_frame_data().get_current_frame_data().commandBuffer;

		if (texture.layout == description.newLayout)
			return;

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


			switch(texture.descriptor.format)
			{
				case TextureFormat::DEPTH_16_UNORM:
					aspectMask = vk::ImageAspectFlagBits::eDepth;
					break;
				
				case TextureFormat::DEPTH_STENCIL24_S8U_INT:
					aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
					break;

				default:
					aspectMask = vk::ImageAspectFlagBits::eColor;
					break;
			}

			subresourceRange.aspectMask = aspectMask;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = texture.descriptor.mipLevels;
			subresourceRange.baseArrayLayer = 0;
			subresourceRange.layerCount = texture.descriptor.arrayLayers;

			imageBarrier.subresourceRange = subresourceRange;
			imageBarrier.image = texture.image;
		}

		vk::DependencyInfo dependencyInfo = vk::DependencyInfo().setImageMemoryBarriers(imageBarrier);

		commandBuffer.pipelineBarrier2(dependencyInfo);
		texture.layout = description.newLayout;
	}

	void transition_texture_layout_imm(RenderInterfaceVulkan& rhi, Texture& texture, const ImageTransitionDescription& description)
	{
		if (texture.layout == description.newLayout)
		{
			// TODO: TUR_LOG_WARN("Attempted to transition to the same image layout.");
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

			switch(texture.descriptor.format)
			{
				case TextureFormat::DEPTH_16_UNORM:
					aspectMask = vk::ImageAspectFlagBits::eDepth;
					break;
				
				case TextureFormat::DEPTH_STENCIL24_S8U_INT:
					aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
					break;

				default:
					aspectMask = vk::ImageAspectFlagBits::eColor;
					break;
			}

			subresourceRange.aspectMask = aspectMask;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = texture.descriptor.mipLevels;
			subresourceRange.baseArrayLayer = 0;
			subresourceRange.layerCount = texture.descriptor.arrayLayers;

			imageBarrier.subresourceRange = subresourceRange;
			imageBarrier.image = texture.image;
		}

		vk::DependencyInfo dependencyInfo = vk::DependencyInfo().setImageMemoryBarriers(imageBarrier);

		utils::submit_immediate_command(
			rhi, [&](vk::CommandBuffer commandBuffer) { commandBuffer.pipelineBarrier2(dependencyInfo); }
		);

		texture.layout = description.newLayout;
	}
}
