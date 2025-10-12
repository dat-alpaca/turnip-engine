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
			utils::transition_texture_layout(
				*rRHI, colorTexture,
				{.newLayout = vk::ImageLayout::eColorAttachmentOptimal,
				 .srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe,
				 .srcAccessMask = vk::AccessFlagBits2::eNone,
				 .dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
				 .dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite}
			);
		}

		if (target.depthAttachment.textureHandle != invalid_handle)
		{
			// TODO: enable depth
			auto& texture = resources.get_textures().get(target.depthAttachment.textureHandle);
			// utils::transition_texture_layout(*rRHI, texture, vk::ImageLayout::eDepthStencilAttachmentOptimal);
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
				colorAttachmentInfo.clearValue =
					vk::ClearValue({mClearColor.color.r, mClearColor.color.g, mClearColor.color.b, mClearColor.color.a}
					);
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

			// TODO: use the 	rendertarget
			renderInfo.renderArea = vk::Rect2D({}, {colorTexture.extent.width, colorTexture.extent.height});
			renderInfo.colorAttachmentCount = 1;
			renderInfo.layerCount = 1;
			renderInfo.pColorAttachments = &colorAttachmentInfo;
			// renderInfo.pDepthAttachment = &depthAttachmentInfo;
			// renderInfo.pStencilAttachment = nullptr;
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

	void CommandBufferVulkan::clear(const ClearColor& clearColor, ClearFlags flags)
	{
		// TODO: use color flags
		mClearColor = clearColor;
	}

	void CommandBufferVulkan::set_viewport(const Viewport& viewport)
	{
		mCommandBuffer.setViewport(
			0,
			vk::Viewport(viewport.x, viewport.y, viewport.width, viewport.height, viewport.minDepth, viewport.maxDepth)
		);
	}
	void CommandBufferVulkan::set_scissor(const Extent2D& scissor)
	{
		mCommandBuffer.setScissor(
			0, vk::Rect2D({(i32)scissor.x, (i32)scissor.y}, {(u32)scissor.width, (u32)scissor.height})
		);
	}

	void CommandBufferVulkan::bind_vertex_buffer(buffer_handle bufferHandle, u32 binding)
	{
		const Buffer& buffer = rRHI->get_resource_handler().get_buffers().get(bufferHandle);
		const vk::DeviceSize offset = 0;

		mCommandBuffer.bindVertexBuffers(binding, buffer.buffer, offset);
	}
	void CommandBufferVulkan::bind_index_buffer(buffer_handle bufferHandle, BufferIndexType type)
	{
		const Buffer& buffer = rRHI->get_resource_handler().get_buffers().get(bufferHandle);
		const vk::DeviceSize offset = 0;

		mCommandBuffer.bindIndexBuffer(buffer.buffer, offset, get_buffer_index_type(type));
	}
	void CommandBufferVulkan::bind_pipeline(pipeline_handle pipelineHandle)
	{
		const Pipeline& pipeline = rRHI->get_resource_handler().get_pipelines().get(pipelineHandle);
		mCommandBuffer.bindPipeline(get_pipeline_type(pipeline.type), pipeline.pipeline);
	}

	void CommandBufferVulkan::draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance)
	{
		mCommandBuffer.draw(vertexCount, instanceCount, firstVertex, firstInstance);
	}
}

namespace tur::vulkan
{
	void CommandBufferVulkan::copy_image(
		vk::Image source, vk::Image target, vk::Extent2D sourceSize, vk::Extent2D targetSize
	)
	{
		vk::ImageBlit2 blitRegion = {};
		{
			blitRegion.srcOffsets[1].x = sourceSize.width;
			blitRegion.srcOffsets[1].y = sourceSize.height;
			blitRegion.srcOffsets[1].z = 1;

			blitRegion.dstOffsets[1].x = targetSize.width;
			blitRegion.dstOffsets[1].y = targetSize.height;
			blitRegion.dstOffsets[1].z = 1;

			blitRegion.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
			blitRegion.srcSubresource.baseArrayLayer = 0;
			blitRegion.srcSubresource.layerCount = 1;
			blitRegion.srcSubresource.mipLevel = 0;

			blitRegion.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
			blitRegion.dstSubresource.baseArrayLayer = 0;
			blitRegion.dstSubresource.layerCount = 1;
			blitRegion.dstSubresource.mipLevel = 0;
		}

		vk::BlitImageInfo2 blitInfo = {};
		{
			blitInfo.srcImage = source;
			blitInfo.dstImage = target;
			blitInfo.srcImageLayout = vk::ImageLayout::eTransferSrcOptimal;
			blitInfo.dstImageLayout = vk::ImageLayout::eTransferDstOptimal;

			blitInfo.filter = vk::Filter::eLinear;
			blitInfo.regionCount = 1;
			blitInfo.pRegions = &blitRegion;
		}

		mCommandBuffer.blitImage2(blitInfo);
	}

	void CommandBufferVulkan::blit_onto_swapchain()
	{
		const auto& currentImage = rRHI->get_frame_data().get_image_buffer_index();
		auto& swapchainImages = rRHI->get_state().swapChainImages;
		auto& swapchainExtent = rRHI->get_state().swapchainExtent;
		auto& mainRenderTarget = rRHI->get_state().mainRenderTarget;

		auto& colorTextureHandle = mainRenderTarget.colorAttachment.textureHandle;
		auto& colorTexture = rRHI->get_resource_handler().get_textures().get(colorTextureHandle);

		utils::transition_texture_layout(
			*rRHI, colorTexture,
			{.newLayout = vk::ImageLayout::eTransferSrcOptimal,
			 .srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
			 .srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
			 .dstStageMask = vk::PipelineStageFlagBits2::eTransfer,
			 .dstAccessMask = vk::AccessFlagBits2::eTransferRead}
		);

		Texture swapchainTexture;
		swapchainTexture.image = swapchainImages.at(currentImage);
		swapchainTexture.layout = vk::ImageLayout::eUndefined;

		utils::transition_texture_layout(
			*rRHI, swapchainTexture,
			{.newLayout = vk::ImageLayout::eTransferDstOptimal,
			 .srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput | vk::PipelineStageFlagBits2::eCopy
							 | vk::PipelineStageFlagBits2::eResolve | vk::PipelineStageFlagBits2::eBlit
							 | vk::PipelineStageFlagBits2::eClear,
			 .srcAccessMask = vk::AccessFlagBits2::eMemoryRead,
			 .dstStageMask = vk::PipelineStageFlagBits2::eTransfer,
			 .dstAccessMask = vk::AccessFlagBits2::eTransferWrite}
		);

		copy_image(
			colorTexture.image, swapchainImages.at(currentImage),
			{colorTexture.extent.width, colorTexture.extent.height}, swapchainExtent
		);

		utils::transition_texture_layout(
			*rRHI, swapchainTexture,
			{.newLayout = vk::ImageLayout::ePresentSrcKHR,
			 .srcStageMask = vk::PipelineStageFlagBits2::eTransfer,
			 .srcAccessMask = vk::AccessFlagBits2::eTransferWrite,
			 .dstStageMask = vk::PipelineStageFlagBits2::eBottomOfPipe,
			 .dstAccessMask = vk::AccessFlagBits2::eNone}
		);
	}
}
