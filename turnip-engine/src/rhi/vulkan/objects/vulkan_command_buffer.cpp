#include "vulkan_command_buffer.hpp"
#include "rhi/vulkan/vulkan_render_interface.hpp"

namespace tur::vulkan
{
	CommandBufferVulkan::CommandBufferVulkan(NON_OWNING RenderInterfaceVulkan* rhi)
		: rRHI(rhi)
	{
	}

	void CommandBufferVulkan::reset(vk::CommandBuffer commandBuffer)
	{
		mCommandBuffer = commandBuffer;
	}

	void CommandBufferVulkan::set_clear_color(const ClearColor& clearColor)
	{
		mClearColor = clearColor;
	}

	void CommandBufferVulkan::begin(render_target_handle renderTargetHandle)
	{
		auto& resources = rRHI->get_resource_handler();

		RenderTarget target;
		if (renderTargetHandle != invalid_handle)
			target = resources.get_render_targets().get(renderTargetHandle);
		else
			target = rRHI->get_state().mainRenderTarget;

		mUsedTarget = renderTargetHandle;

		auto& colorTexture = resources.get_textures().get(target.colorAttachment.textureHandle);

		{
			utils::transition_texture_layout(*rRHI, colorTexture, vk::ImageLayout::eColorAttachmentOptimal);
		}

		if (target.depthAttachment.textureHandle != invalid_handle)
		{
			auto& texture = resources.get_textures().get(target.depthAttachment.textureHandle);
			utils::transition_texture_layout(*rRHI, texture, vk::ImageLayout::eDepthStencilAttachmentOptimal);
		}

		// 3. create rendering info
		vk::RenderingInfo renderInfo = {};
		{
			vk::RenderingAttachmentInfo colorAttachmentInfo = {};
			{
				colorAttachmentInfo.imageView = colorTexture.imageView;
				colorAttachmentInfo.imageLayout = vk::ImageLayout::eAttachmentOptimal;
				colorAttachmentInfo.resolveMode = vk::ResolveModeFlagBits::eNone;
				colorAttachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
				colorAttachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
				colorAttachmentInfo.clearValue = vk::ClearValue(
					{mClearColor.color.r, mClearColor.color.g, mClearColor.color.b, mClearColor.color.a});
			}

			vk::RenderingAttachmentInfo depthAttachmentInfo = {};
			{
				depthAttachmentInfo.imageView = colorTexture.imageView;
				depthAttachmentInfo.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
				depthAttachmentInfo.resolveMode = vk::ResolveModeFlagBits::eNone;
				depthAttachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
				depthAttachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
				depthAttachmentInfo.clearValue =
					vk::ClearValue(vk::ClearDepthStencilValue(mClearColor.depth, mClearColor.stencil));
			}

			// TODO: use the framebuffer instead, or the general rendertarget
			renderInfo.renderArea = vk::Rect2D({}, {colorTexture.extent.width, colorTexture.extent.height});
			renderInfo.colorAttachmentCount = 1;
			renderInfo.layerCount = 1;
			renderInfo.pColorAttachments = &colorAttachmentInfo;
			// renderInfo.pDepthAttachment = &depthAttachmentInfo;
			renderInfo.pStencilAttachment = nullptr;
		}

		mCommandBuffer.beginRendering(renderInfo);
	}
	void CommandBufferVulkan::end()
	{
		mCommandBuffer.endRendering();

		if (mUsedTarget != invalid_handle)
			return;

		blit_onto_swapchain();
	}

	void CommandBufferVulkan::copy_image(vk::Image source, vk::Image target, vk::Extent2D sourceSize,
										 vk::Extent2D targetSize)
	{
	}

	void CommandBufferVulkan::blit_onto_swapchain()
	{
		const auto& currentImage = rRHI->get_frame_data().get_image_buffer_index();
		auto& swapchainImages = rRHI->get_state().swapChainImages;
		auto& swapchainExtent = rRHI->get_state().swapchainExtent;
		auto& mainRenderTarget = rRHI->get_state().mainRenderTarget;

		auto& colorTextureHandle = mainRenderTarget.colorAttachment.textureHandle;
		auto& colorTexture = rRHI->get_resource_handler().get_textures().get(colorTextureHandle);

		utils::transition_texture_layout(*rRHI, colorTexture, vk::ImageLayout::eTransferSrcOptimal);

		Texture swapchainTexture;
		swapchainTexture.image = swapchainImages.at(currentImage);
		swapchainTexture.layout = vk::ImageLayout::eUndefined;
		utils::transition_texture_layout(*rRHI, swapchainTexture, vk::ImageLayout::eTransferDstOptimal);

		copy_image(colorTexture.image, swapchainImages.at(currentImage),
				   {colorTexture.extent.width, colorTexture.extent.height}, swapchainExtent);

		utils::transition_texture_layout(*rRHI, swapchainTexture, vk::ImageLayout::ePresentSrcKHR);
	}
}
