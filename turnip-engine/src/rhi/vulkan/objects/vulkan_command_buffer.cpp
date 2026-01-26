#include "assert/assert.hpp"
#include "rhi/vulkan/allocators/command_buffer.hpp"
#include "rhi/vulkan/objects/render_target.hpp"
#include "rhi/vulkan/vulkan_render_interface.hpp"

#include "vulkan_command_buffer.hpp"
#include "vulkan/vulkan.hpp"
#include "defines.hpp"

namespace tur::vulkan
{
	CommandBufferVulkan::CommandBufferVulkan(NON_OWNING RenderInterfaceVulkan* rhi)
		: rRHI(rhi)
	{
		TUR_ASS(rRHI);
	}

	void CommandBufferVulkan::initialize_secondary()
	{
		mCommandBuffer = allocate_single_secondary_command_buffer(rRHI->get_state().logicalDevice, rRHI->get_state().commandPool);
		mIsSecondary = true;
	}
	void CommandBufferVulkan::reset(vk::CommandBuffer commandBuffer)
	{
		TUR_ASS(commandBuffer);

		mCommandBuffer = commandBuffer;
		mIsSecondary = false;
	}
	void CommandBufferVulkan::execute_secondary_buffers(std::span<vk::CommandBuffer> buffers)
	{
		TUR_ASSERT(!mIsSecondary, "Attempted to execute secondary buffers inside a non-primary command buffer");
		mCommandBuffer.executeCommands(buffers.size(), buffers.data());
	}

	void CommandBufferVulkan::begin(render_target_handle renderTargetHandle)
	{
		auto& resources = rRHI->get_resource_handler();
		mUsedTarget = renderTargetHandle;
	
		// Secondary command buffer setup:
		vk::CommandBufferInheritanceRenderingInfo renderingInfo = {};
		
		vk::CommandBufferInheritanceInfo inheritanceInfo = {};
		if(mIsSecondary)
		{
			renderingInfo.sType = vk::StructureType::eCommandBufferInheritanceRenderingInfo;
			inheritanceInfo.sType = vk::StructureType::eCommandBufferInheritanceInfo;
			
			RenderTarget target = get_render_target();
			auto& colorTexture = resources.get_textures().get(target.colorAttachment.textureHandle);
			auto& depthStencilTexture = resources.get_textures().get(target.depthStencilAttachment.textureHandle);

			renderingInfo.colorAttachmentCount = 1;
			renderingInfo.pColorAttachmentFormats = &colorTexture.format;
			renderingInfo.depthAttachmentFormat = depthStencilTexture.format;
			renderingInfo.stencilAttachmentFormat = depthStencilTexture.format;
			renderingInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;

			inheritanceInfo.pNext = &renderingInfo;
		}

		// Command buffer begin setup:
		vk::CommandBufferBeginInfo beginInfo = {};
		{
			beginInfo.pInheritanceInfo = (mIsSecondary) ? &inheritanceInfo : nullptr;
			beginInfo.flags = (mIsSecondary)
				? vk::CommandBufferUsageFlagBits::eRenderPassContinue
				: vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
		}

		auto result = mCommandBuffer.begin(beginInfo);
		if (result != vk::Result::eSuccess)
			TUR_LOG_ERROR("Failed to begin() recording to vulkan command buffer.", static_cast<i32>(result));
	}
	void CommandBufferVulkan::begin_rendering()
	{
		auto& resources = rRHI->get_resource_handler();

		RenderTarget target = get_render_target();
		auto& colorTexture = resources.get_textures().get(target.colorAttachment.textureHandle);
		auto& depthStencilTexture = resources.get_textures().get(target.depthStencilAttachment.textureHandle);

		utils::transition_texture_layout(
			*rRHI, colorTexture,
			{
				.newLayout = vk::ImageLayout::eColorAttachmentOptimal,
				.srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe,
				.srcAccessMask = vk::AccessFlagBits2::eNone,
				.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
				.dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite
			}
		);

		if (target.depthStencilAttachment.textureHandle != invalid_handle)
		{
			utils::transition_texture_layout(
				*rRHI, depthStencilTexture,
				{
					.newLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
					.srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe,
					.srcAccessMask = vk::AccessFlagBits2::eNone,
					.dstStageMask = vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
					.dstAccessMask = vk::AccessFlagBits2::eDepthStencilAttachmentRead | vk::AccessFlagBits2::eDepthStencilAttachmentWrite
				}
			);
		}

		// 3. create rendering info
		vk::RenderingInfo renderInfo = {};
		vk::RenderingAttachmentInfo colorAttachmentInfo = {}, depthStencilAttachmentInfo = {};
		{
			colorAttachmentInfo.imageView = colorTexture.imageView;
			colorAttachmentInfo.imageLayout = vk::ImageLayout::eAttachmentOptimal;
			colorAttachmentInfo.resolveMode = vk::ResolveModeFlagBits::eNone;
			colorAttachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
			colorAttachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
			colorAttachmentInfo.clearValue =
				vk::ClearValue({mClearColor.color.r, mClearColor.color.g, mClearColor.color.b, mClearColor.color.a}
			);

			depthStencilAttachmentInfo.imageView = depthStencilTexture.imageView;
			depthStencilAttachmentInfo.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
			depthStencilAttachmentInfo.resolveMode = vk::ResolveModeFlagBits::eNone;
			depthStencilAttachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
			depthStencilAttachmentInfo.storeOp = vk::AttachmentStoreOp::eDontCare;
			depthStencilAttachmentInfo.clearValue =
				vk::ClearValue(vk::ClearDepthStencilValue(mClearColor.depth, mClearColor.stencil));

			// TODO: use the 	rendertarget
			renderInfo.flags |= vk::RenderingFlagBits::eContentsSecondaryCommandBuffers;
			renderInfo.renderArea = vk::Rect2D({}, {colorTexture.extent.width, colorTexture.extent.height});
			renderInfo.colorAttachmentCount = 1;
			renderInfo.layerCount = 1;
			renderInfo.pColorAttachments = &colorAttachmentInfo;
			renderInfo.pDepthAttachment = &depthStencilAttachmentInfo;
			renderInfo.pStencilAttachment = &depthStencilAttachmentInfo;
		}

		mCommandBuffer.beginRendering(renderInfo);
	}
	void CommandBufferVulkan::end_rendering()
	{
		mBoundPipeline = invalid_handle;
		mCommandBuffer.endRendering();
	}
	void CommandBufferVulkan::end()
	{
		vk::Result result = mCommandBuffer.end();
		if (result != vk::Result::eSuccess)
			TUR_LOG_CRITICAL("Failed to end() recording to vulkan command buffer.");
	}
	void CommandBufferVulkan::blit()
	{
		blit_onto_swapchain();
	}

	void CommandBufferVulkan::set_clear_color(const ClearColor& clearColor, ClearFlags flags)
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

		mBoundPipeline = pipelineHandle;
	}
	void CommandBufferVulkan::bind_descriptor_set(descriptor_set_handle setHandle)
	{
		auto& resources = rRHI->get_resource_handler();
		const DescriptorSet& set = resources.get_descriptor_sets().get(setHandle);
		const Pipeline& pipeline = resources.get_pipelines().get(mBoundPipeline);

		mCommandBuffer.bindDescriptorSets(get_pipeline_type(pipeline.type), pipeline.layout, 0, {set.set}, {});
	}

	void CommandBufferVulkan::draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance)
	{
		mCommandBuffer.draw(vertexCount, instanceCount, firstVertex, firstInstance);
	}
	void CommandBufferVulkan::draw_indexed(u32 indexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance)
	{
		mCommandBuffer.drawIndexed(indexCount, instanceCount, 0, firstVertex, firstInstance);
	}
}

namespace tur::vulkan
{
	RenderTarget CommandBufferVulkan::get_render_target()
	{
		auto& resources = rRHI->get_resource_handler();

		RenderTarget target;
		if (mUsedTarget != invalid_handle)
			return resources.get_render_targets().get(mUsedTarget);
		else
			return rRHI->get_state().mainRenderTarget;
	}

	void CommandBufferVulkan::copy_image(vk::Image source, vk::Image target, vk::Extent2D sourceSize, vk::Extent2D targetSize)
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
			blitRegion.srcSubresource.layerCount = 1	;
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
		if(mUsedTarget != invalid_handle)
			return;

		const auto& currentImage = rRHI->get_frame_data().get_image_buffer_index();
		auto& swapchainTexture = rRHI->get_state().swapchainTextures.at(currentImage);
		auto& swapchainExtent = rRHI->get_state().swapchainExtent;
		
		auto& mainRenderTarget = rRHI->get_state().mainRenderTarget;
		auto& colorTextureHandle = mainRenderTarget.colorAttachment.textureHandle;
		auto& colorTexture = rRHI->get_resource_handler().get_textures().get(colorTextureHandle);

		utils::transition_texture_layout(
			*rRHI, colorTexture,
			{
				.newLayout = vk::ImageLayout::eTransferSrcOptimal,
			 	.srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
			 	.srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
			 	.dstStageMask = vk::PipelineStageFlagBits2::eTransfer,
			 	.dstAccessMask = vk::AccessFlagBits2::eTransferRead
			}
		);

		utils::transition_texture_layout(
			*rRHI, swapchainTexture, {
				.newLayout = vk::ImageLayout::eTransferDstOptimal,
				.srcStageMask = vk::PipelineStageFlagBits2::eAllCommands,
				.srcAccessMask = vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite,
				.dstStageMask = vk::PipelineStageFlagBits2::eTransfer,
				.dstAccessMask = vk::AccessFlagBits2::eTransferWrite
			}
		);

		copy_image(
			colorTexture.image, swapchainTexture.image, {colorTexture.extent.width, colorTexture.extent.height},
			swapchainExtent
		);

		utils::transition_texture_layout(
			*rRHI, swapchainTexture, {
				.newLayout = vk::ImageLayout::ePresentSrcKHR,
			 	.srcStageMask = vk::PipelineStageFlagBits2::eTransfer,
			 	.srcAccessMask = vk::AccessFlagBits2::eTransferWrite,
			 	.dstStageMask = vk::PipelineStageFlagBits2::eBottomOfPipe,
			 	.dstAccessMask = vk::AccessFlagBits2::eNone
			}
		);
	}
}
