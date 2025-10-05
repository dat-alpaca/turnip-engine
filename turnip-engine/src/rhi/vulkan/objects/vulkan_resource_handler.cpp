#include "vulkan_resource_handler.hpp"
#include "rhi/vulkan/vulkan_render_interface.hpp"

namespace tur::vulkan
{
	void ResourceHandler::initialize(RenderInterfaceVulkan* rhi)
	{
		rRHI = rhi;
		rDevice = rhi->get_state().logicalDevice;
	}
	void ResourceHandler::destroy()
	{
		for (const auto& shaderHandle : mShaders.available_indices())
			destroy_shader(shaderHandle);

		for (const auto& bufferHandle : mBuffers.available_indices())
			destroy_buffer(bufferHandle);

		for (const auto& textureHandle : mTextures.available_indices())
			destroy_texture(textureHandle);

		for (const auto& pipelineHandle : mPipelines.available_indices())
			destroy_graphics_pipeline(pipelineHandle);
	}

	shader_handle ResourceHandler::create_shader(const ShaderDescriptor& descriptor)
	{
		vk::ShaderModule shaderModule = allocate_shader_module(rDevice, descriptor);
		return mShaders.add(shaderModule);
	}
	void ResourceHandler::destroy_shader(shader_handle shaderHandle)
	{
		vk::ShaderModule shaderModule = mShaders.get(shaderHandle);
		rDevice.destroyShaderModule(shaderModule);
		mShaders.remove(shaderHandle);
	}

	render_target_handle ResourceHandler::create_render_target(const RenderTargetDescriptor& descriptor)
	{
		// create textures:
		RenderTarget renderTarget;
		renderTarget.colorAttachment.loadOp = descriptor.attachmentDescriptions[0].loadOp;
		renderTarget.colorAttachment.storeOp = descriptor.attachmentDescriptions[0].storeOp;
		renderTarget.colorAttachment.textureHandle =
			create_empty_texture(descriptor.attachmentDescriptions[0].textureDescriptor);

		// renderTarget.depthAttachment.loadOp = descriptor.attachmentDescriptions[1].loadOp;
		// renderTarget.depthAttachment.storeOp = descriptor.attachmentDescriptions[1].storeOp;

		return mRenderTargets.add(renderTarget);
	}
	void ResourceHandler::destroy_render_target(render_target_handle renderTargetHandle)
	{
		RenderTarget renderTarget = mRenderTargets.get(renderTargetHandle);

		if (renderTarget.colorAttachment.textureHandle != invalid_handle)
			destroy_texture(renderTarget.colorAttachment.textureHandle);

		if (renderTarget.depthAttachment.textureHandle != invalid_handle)
			destroy_texture(renderTarget.depthAttachment.textureHandle);
	}

	pipeline_handle ResourceHandler::create_graphics_pipeline(const PipelineDescriptor& descriptor)
	{
		Pipeline pipeline = allocate_graphics_pipeline(rRHI, descriptor);

		destroy_shader(descriptor.vertexShader);
		destroy_shader(descriptor.fragmentShader);

		if (descriptor.tesselationControlShader != invalid_handle)
			destroy_shader(descriptor.tesselationControlShader);

		if (descriptor.tesselationEvaluationShader != invalid_handle)
			destroy_shader(descriptor.tesselationEvaluationShader);

		if (descriptor.geometryShader != invalid_handle)
			destroy_shader(descriptor.geometryShader);

		return mPipelines.add(pipeline);
	}
	void ResourceHandler::destroy_graphics_pipeline(pipeline_handle pipelineHandle)
	{
		rRHI->get_deletion_queue().submit_pipeline(pipelineHandle);
	}

	buffer_handle ResourceHandler::create_buffer(const BufferDescriptor& descriptor, const std::vector<byte>& data)
	{
		auto& vmaAllocator = rRHI->get_state().vmaAllocator;

		// Source Buffer:
		BufferDescriptor srcDescriptor = {};
		{
			srcDescriptor.usage = descriptor.usage;
			srcDescriptor.type = descriptor.type | BufferType::TRANSFER_DST;
			srcDescriptor.memoryUsage = BufferMemoryUsage::GPU_ONLY;
		}
		Buffer buffer = allocate_buffer(vmaAllocator, srcDescriptor, data.size());

		// Staging buffer:
		BufferDescriptor stagingDescriptor = {};
		{
			stagingDescriptor.memoryUsage = BufferMemoryUsage::CPU_ONLY;
			stagingDescriptor.type = BufferType::TRANSFER_SRC;
		}
		Buffer stagingBuffer = allocate_buffer(vmaAllocator, stagingDescriptor, data.size());

		// Buffer data:
		if (data.data())
		{
			void* bufferData = nullptr;
			vmaMapMemory(vmaAllocator, stagingBuffer.allocation, &bufferData);
			std::memcpy(bufferData, (u8*)data.data(), data.size());
			vmaUnmapMemory(vmaAllocator, stagingBuffer.allocation);
		}

		// Buffer copy:
		utils::submit_immediate_command(*rRHI,
										[&](vk::CommandBuffer commandBuffer)
										{
											vk::BufferCopy region;
											{
												region.dstOffset = 0;
												region.srcOffset = 0;
												region.size = data.size();
											}

											commandBuffer.copyBuffer(stagingBuffer.buffer, buffer.buffer, region);
										});

		vmaDestroyBuffer(vmaAllocator, stagingBuffer.buffer, stagingBuffer.allocation);
		return mBuffers.add(buffer);
	}
	buffer_handle ResourceHandler::create_empty_buffer(const BufferDescriptor& descriptor, u32 size)
	{
		return mBuffers.add(allocate_buffer(rRHI->get_state().vmaAllocator, descriptor, size));
	}
	void ResourceHandler::update_buffer(buffer_handle bufferHandle, const std::vector<byte>& data, u32 offset)
	{
		const Buffer& targetBuffer = mBuffers.get(bufferHandle);
		auto& vmaAllocator = rRHI->get_state().vmaAllocator;

		// Staging buffer:
		BufferDescriptor stagingDescriptor = {};
		{
			stagingDescriptor.memoryUsage = BufferMemoryUsage::CPU_ONLY;
			stagingDescriptor.type = BufferType::TRANSFER_SRC;
		}
		Buffer stagingBuffer = allocate_buffer(vmaAllocator, stagingDescriptor, data.size());

		{
			void* bufferData = nullptr;
			vmaMapMemory(vmaAllocator, stagingBuffer.allocation, &bufferData);
			std::memcpy(bufferData, (u8*)data.data() + offset, data.size());
			vmaUnmapMemory(vmaAllocator, stagingBuffer.allocation);
		}

		// Buffer copy:
		utils::submit_immediate_command(*rRHI,
										[&](vk::CommandBuffer commandBuffer)
										{
											vk::BufferCopy region;
											{
												region.dstOffset = 0;
												region.srcOffset = 0;
												region.size = data.size();
											}

											commandBuffer.copyBuffer(stagingBuffer.buffer, targetBuffer.buffer, region);
										});

		vmaDestroyBuffer(vmaAllocator, stagingBuffer.buffer, stagingBuffer.allocation);
	}
	void ResourceHandler::copy_buffer(buffer_handle source, buffer_handle destination, u32 size, u32 srcOffset,
									  u32 dstOffset)
	{
		utils::submit_immediate_command(*rRHI,
										[&](vk::CommandBuffer commandBuffer)
										{
											vk::BufferCopy region;
											{
												region.dstOffset = dstOffset;
												region.srcOffset = srcOffset;
												region.size = size;
											}

											commandBuffer.copyBuffer(mBuffers.get(source).buffer,
																	 mBuffers.get(destination).buffer, region);
										});
	}
	void ResourceHandler::copy_buffer_to_texture(buffer_handle source, texture_handle destination, u32 width,
												 u32 height, u32 depth)
	{
		const Buffer& buffer = mBuffers.get(source);
		const Texture& texture = mTextures.get(destination);

		utils::submit_immediate_command(*rRHI,
										[&](vk::CommandBuffer commandBuffer)
										{
											vk::ImageAspectFlags aspectMask = vk::ImageAspectFlagBits::eColor;

											vk::BufferImageCopy2 region;
											{
												region.bufferOffset = 0;
												region.bufferRowLength = 0;
												region.bufferImageHeight = 0;

												region.imageSubresource.aspectMask = aspectMask;
												region.imageSubresource.mipLevel = 0;
												region.imageSubresource.baseArrayLayer = 0;
												region.imageSubresource.layerCount = 1;
												region.imageOffset = vk::Offset3D(0, 0, 0);
												region.imageExtent = vk::Extent3D(width, height, depth);
											}

											vk::CopyBufferToImageInfo2 copyInfo = {};
											{
												copyInfo.srcBuffer = buffer.buffer;
												copyInfo.dstImage = texture.image;
												copyInfo.dstImageLayout = vk::ImageLayout::eTransferDstOptimal;
												copyInfo.regionCount = 1;
												copyInfo.pRegions = &region;
											}

											commandBuffer.copyBufferToImage2(copyInfo);
										});
	}
	void ResourceHandler::destroy_buffer(buffer_handle bufferHandle)
	{
		rRHI->get_deletion_queue().submit_buffer(bufferHandle);
	}

	texture_handle ResourceHandler::create_texture(const TextureDescriptor& descriptor, const TextureAsset& asset)
	{
		auto& state = rRHI->get_state();
		auto& device = state.logicalDevice;
		Texture texture = allocate_texture(state.logicalDevice, state.vmaAllocator, descriptor);
		texture_handle handle = mTextures.add(texture);

		// Data:
		if (asset.data.size())
			update_texture(handle, asset);

		else
			utils::transition_texture_layout(*rRHI, texture, vk::ImageLayout::eShaderReadOnlyOptimal);

		return handle;
	}
	texture_handle ResourceHandler::create_empty_texture(const TextureDescriptor& descriptor)
	{
		return mTextures.add(
			allocate_texture(rRHI->get_state().logicalDevice, rRHI->get_state().vmaAllocator, descriptor));
	}
	void ResourceHandler::update_texture(texture_handle textureHandle, const TextureAsset& asset)
	{
		Texture& texture = mTextures.get(textureHandle);
		BufferDescriptor stagingDescriptor = {};
		{
			stagingDescriptor.memoryUsage = BufferMemoryUsage::CPU_ONLY;
			stagingDescriptor.type = BufferType::TRANSFER_SRC | BufferType::TRANSFER_DST;
		}

		buffer_handle stagingBufferHandle = create_buffer(stagingDescriptor, asset.data);

		// TODO: prepare other image layouts
		{
			utils::transition_texture_layout(*rRHI, texture, vk::ImageLayout::eTransferDstOptimal);

			copy_buffer_to_texture(stagingBufferHandle, textureHandle, asset.width, asset.height);

			utils::transition_texture_layout(*rRHI, texture, vk::ImageLayout::eShaderReadOnlyOptimal);
		}

		destroy_buffer(stagingBufferHandle);
	}
	void ResourceHandler::destroy_texture(texture_handle textureHandle)
	{
		rRHI->get_deletion_queue().submit_texture(textureHandle);
	}
}