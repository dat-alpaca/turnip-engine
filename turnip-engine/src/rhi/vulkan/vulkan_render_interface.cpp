#include "vulkan_render_interface.hpp"

#include <optional>
#include <sys/wait.h>
#include <vector>
#include "defines.hpp"
#include "graphics/objects/sync.hpp"

#include "rhi/vulkan/allocators/allocators.hpp"
#include "rhi/vulkan/initializers/initializers.hpp"
#include "rhi/vulkan/objects/pipeline.hpp"
#include "rhi/vulkan/objects/texture.hpp"
#include "rhi/vulkan/objects/vulkan_command_buffer.hpp"
#include "vulkan/vulkan.hpp"

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

	void RenderInterfaceVulkan::begin_frame(fence_handle inFlightFence, u32 fenceTimeout)
	{
		auto& device = mState.logicalDevice;
		auto& fence = mResources.get_fences().get(inFlightFence);

		vk::Result result = device.waitForFences({ fence }, true, fenceTimeout);
		if (result != vk::Result::eSuccess)
			TUR_LOG_CRITICAL("Failed to wait for fences.");

		mDeletionQueue.flush();
	}
	void RenderInterfaceVulkan::reset_fence(fence_handle inFlightFence)
	{
		auto& device = mState.logicalDevice;
		auto& fence = mResources.get_fences().get(inFlightFence);

		auto _ = device.resetFences({ fence });
	}

	std::optional<image_handle> RenderInterfaceVulkan::acquire_swapchain_image(semaphore_handle signalImageReady, u32 imageReadyTimeout)
	{
		auto& device = mState.logicalDevice;
		auto& swapchain = mState.swapchain;

		auto& imageReadySemaphore = mResources.get_semaphores().get(signalImageReady);

		u32 imageIndex = 0;
		VkResult result = vkAcquireNextImageKHR(device, swapchain, imageReadyTimeout, imageReadySemaphore, nullptr, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			utils::recreate_swapchain(*this);
			return std::nullopt;
		}

		else if (result == VK_SUCCESS || result == VK_NOT_READY || result == VK_TIMEOUT)
		{
			/* Blank */
		}

		else
			TUR_LOG_CRITICAL("Failed to acquire swapchain image");
		
		return imageIndex;
	}

	void RenderInterfaceVulkan::submit(queue_handle graphicsQueue, command_buffer_handle commandBuffer, fence_handle inFlightFence, const std::vector<SubmitSemaphoreData>& waitFor, semaphore_handle signal)
	{
		TUR_ASS(commandBuffer != invalid_handle);

		auto& currentCommandBuffer = mResources.get_command_buffers().get(commandBuffer);
		auto& fence = mResources.get_fences().get(inFlightFence);

		// Semaphores:
		std::vector<vk::Semaphore> waitForSemaphores;
		std::vector<vk::PipelineStageFlags> stageFlags;
		waitForSemaphores.reserve(waitFor.size());
		stageFlags.reserve(waitFor.size());
		for(const auto& [semaphore, flags] : waitFor)
		{
			waitForSemaphores.push_back(mResources.get_semaphores().get(semaphore));
			stageFlags.push_back(get_pipeline_stage_flags(flags));
		}

		// Submit information:
		vk::Queue queue = mState.queueList.get_queue(graphicsQueue).queue;
		vk::Semaphore signalSemaphores[] = { mResources.get_semaphores().get(signal) };
		vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
		vk::SubmitInfo submitInfo = {};
		{
			submitInfo.pWaitDstStageMask = stageFlags.data();

			submitInfo.waitSemaphoreCount = waitForSemaphores.size();
			submitInfo.pWaitSemaphores = waitForSemaphores.data();

			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = signalSemaphores;

			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &currentCommandBuffer;
		}

		vk::Result result = queue.submit(submitInfo, fence);
		if (result != vk::Result::eSuccess)
			TUR_LOG_ERROR("Failed to submit commands to the graphics queue.");
	}
	void RenderInterfaceVulkan::present(queue_handle presentQueue, const std::vector<semaphore_handle>& waitFor, image_handle imageHandle)
	{
		std::vector<vk::Semaphore> waitForSemaphores;
		waitForSemaphores.reserve(waitFor.size());
		for(const auto& semaphore : waitFor)
			waitForSemaphores.push_back(mResources.get_semaphores().get(semaphore));

		vk::Queue queue = mState.queueList.get_queue(presentQueue).queue;

		u32 imageIndices[] = { imageHandle };

		vk::PresentInfoKHR presentInfo = {};
		{
			presentInfo.waitSemaphoreCount = waitForSemaphores.size();
			presentInfo.pWaitSemaphores = waitForSemaphores.data();
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

		rhi.create_main_render_target();
	}

	void transition_texture_layout(RenderInterfaceVulkan& rhi, CommandBufferVulkan commandBuffer, Texture& texture, const ImageTransitionDescription& description)
	{
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

		commandBuffer.get_buffer().pipelineBarrier2(dependencyInfo);
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
