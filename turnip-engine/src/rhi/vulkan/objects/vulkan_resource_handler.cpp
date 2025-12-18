#include "vulkan_resource_handler.hpp"
#include "rhi/vulkan/vulkan_render_interface.hpp"
#include <vk_mem_alloc.h>

namespace tur::vulkan
{
	void ResourceHandler::initialize(RenderInterfaceVulkan* rhi)
	{
		TUR_ASS(rhi);

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

		for (const auto& setLayoutHandle : mSetLayouts.available_indices())
			destroy_descriptor_set_layout(setLayoutHandle);
	}

	shader_handle ResourceHandler::create_shader(const ShaderDescriptor& descriptor)
	{
		vk::ShaderModule shaderModule = allocate_shader_module(rDevice, descriptor);
		return mShaders.add(shaderModule);
	}
	void ResourceHandler::destroy_shader(shader_handle shaderHandle)
	{
		TUR_ASSERT(shaderHandle != invalid_handle, "Invalid shader_handle");

		vk::ShaderModule shaderModule = mShaders.get(shaderHandle);
		rDevice.destroyShaderModule(shaderModule);
		mShaders.remove(shaderHandle);
	}

	render_target_handle ResourceHandler::create_render_target(const RenderTargetDescriptor& descriptor)
	{
		RenderTarget renderTarget;
		renderTarget.colorAttachment.loadOp = descriptor.attachmentDescriptions[0].loadOp;
		renderTarget.colorAttachment.storeOp = descriptor.attachmentDescriptions[0].storeOp;
		renderTarget.colorAttachment.textureHandle =
			create_empty_texture(descriptor.attachmentDescriptions[0].textureDescriptor);

		// TODO: support depth and stencil
		// renderTarget.depthAttachment.loadOp = descriptor.attachmentDescriptions[1].loadOp;
		// renderTarget.depthAttachment.storeOp = descriptor.attachmentDescriptions[1].storeOp;

		return mRenderTargets.add(renderTarget);
	}
	void ResourceHandler::destroy_render_target(render_target_handle renderTargetHandle)
	{
		TUR_ASSERT(renderTargetHandle != invalid_handle, "Invalid render_target_handle");
		RenderTarget renderTarget = mRenderTargets.get(renderTargetHandle);

		if (renderTarget.colorAttachment.textureHandle != invalid_handle)
			destroy_texture(renderTarget.colorAttachment.textureHandle);

		if (renderTarget.depthAttachment.textureHandle != invalid_handle)
			destroy_texture(renderTarget.depthAttachment.textureHandle);
	}

	descriptor_set_layout_handle
	ResourceHandler::create_descriptor_set_layout(const DescriptorSetLayoutDescriptor& descriptorSetLayout)
	{
		// TODO: flexible number of max descriptor sets

		auto& device = rRHI->get_state().logicalDevice;
		DescriptorSetLayout setLayout
		{
			.layoutDescriptor = descriptorSetLayout,
			.layout = allocate_set_layout(device, descriptorSetLayout),
			.pool = allocate_descriptor_pool(device, descriptorSetLayout, 1000)
		};

		return mSetLayouts.add(setLayout);
	}
	void ResourceHandler::destroy_descriptor_set_layout(descriptor_set_layout_handle descriptorSetLayoutHandle)
	{
		TUR_ASSERT(descriptorSetLayoutHandle != invalid_handle, "Invalid descriptor_set_layout_handle");
		rRHI->get_deletion_queue().submit_descriptor_set_layout(descriptorSetLayoutHandle);
	}

	descriptor_set_handle ResourceHandler::create_descriptor_set(descriptor_set_layout_handle layoutHandle)
	{
		TUR_ASSERT(layoutHandle != invalid_handle, "Invalid descriptor_set_layout_handle");

		DescriptorSetLayout setLayout = mSetLayouts.get(layoutHandle);
		vk::DescriptorSet descriptorSet =
			allocate_set_single_layout(rRHI->get_state().logicalDevice, setLayout.pool, setLayout.layout);

		DescriptorSet set;
		{
			set.set = descriptorSet;
			set.layoutHandle = layoutHandle;
		}
		return mDescriptorSets.add(set);
	}

	void ResourceHandler::write_uniform_buffer_to_set(
		descriptor_set_handle setHandle, buffer_handle bufferHandle, const Range& range, u32 binding
	)
	{
		TUR_ASSERT(setHandle != invalid_handle, "Invalid descriptor_set_handle");
		TUR_ASSERT(bufferHandle != invalid_handle, "Invalid buffer_handle");

		const Buffer& buffer = mBuffers.get(bufferHandle);

		vk::DescriptorBufferInfo bufferInfo = {};
		{
			bufferInfo.buffer = buffer.buffer;
			bufferInfo.offset = range.offset;
			bufferInfo.range = range.size;
		}

		vk::WriteDescriptorSet descriptorWrite = {};
		{
			descriptorWrite.dstSet = mDescriptorSets.get(setHandle).set;
			descriptorWrite.dstBinding = binding;
			descriptorWrite.dstArrayElement = 0; // TODO: allow modification
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
			descriptorWrite.pBufferInfo = &bufferInfo;
		}

		rRHI->get_state().logicalDevice.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
	}
	void ResourceHandler::write_storage_buffer_to_set(
		descriptor_set_handle setHandle, buffer_handle bufferHandle, const Range& range, u32 binding
	)
	{
		TUR_ASSERT(setHandle != invalid_handle, "Invalid descriptor_set_handle");
		TUR_ASSERT(bufferHandle != invalid_handle, "Invalid buffer_handle");

		const Buffer& buffer = mBuffers.get(bufferHandle);

		vk::DescriptorBufferInfo bufferInfo = {};
		{
			bufferInfo.buffer = buffer.buffer;
			bufferInfo.offset = range.offset;
			bufferInfo.range = range.size;
		}

		vk::WriteDescriptorSet descriptorWrite = {};
		{
			descriptorWrite.dstSet = mDescriptorSets.get(setHandle).set;
			descriptorWrite.dstBinding = binding;
			descriptorWrite.dstArrayElement = 0; // TODO: allow modification
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.descriptorType = vk::DescriptorType::eStorageBuffer;
			descriptorWrite.pBufferInfo = &bufferInfo;
		}

		rRHI->get_state().logicalDevice.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
	}
	void ResourceHandler::write_texture_to_set(descriptor_set_handle setHandle, texture_handle textureHandle, u32 binding)
	{
		TUR_ASSERT(setHandle != invalid_handle, "Invalid descriptor_set_handle");
		TUR_ASSERT(textureHandle != invalid_handle, "Invalid texture_handle");

		const auto& texture = mTextures.get(textureHandle);
		TUR_ASSERT(
			texture.layout == vk::ImageLayout::eShaderReadOnlyOptimal,
			"Invalid texture layout passed to descriptor set."
		);

		vk::DescriptorImageInfo imageInfo = {};
		{
			imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			imageInfo.imageView = texture.imageView;
			imageInfo.sampler = texture.sampler;
		}

		vk::WriteDescriptorSet descriptorWrite = {};
		{
			descriptorWrite.dstSet = mDescriptorSets.get(setHandle).set;
			descriptorWrite.dstBinding = binding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
			descriptorWrite.descriptorCount = 1;

			descriptorWrite.pImageInfo = &imageInfo;
		}

		rRHI->get_state().logicalDevice.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
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
		TUR_ASSERT(pipelineHandle != invalid_handle, "Invalid pipeline_handle");
		rRHI->get_deletion_queue().submit_pipeline(pipelineHandle);
	}

	buffer_handle ResourceHandler::create_buffer(const BufferDescriptor& descriptor, const void* data, Range range)
	{
		TUR_ASSERT(data, "Invalid data pointer");
		TUR_ASSERT(range.size > 0, "Invalid range size");
		TUR_ASSERT(range.offset >= 0, "Invalid offset size");
		TUR_ASSERT(range.offset + range.size <= range.size, "Invalid range offset");

		auto& vmaAllocator = rRHI->get_state().vmaAllocator;

		// Source Buffer:
		BufferDescriptor srcDescriptor = {};
		{
			srcDescriptor.type = descriptor.type | BufferType::TRANSFER_DST;
			srcDescriptor.usage = descriptor.usage | BufferUsage::PERSISTENT;
			srcDescriptor.memoryUsage = BufferMemoryUsage::GPU_ONLY;
		}
		Buffer buffer = allocate_buffer(vmaAllocator, srcDescriptor, range.size);

		// Staging buffer:
		BufferDescriptor stagingDescriptor = {};
		{
			stagingDescriptor.type = BufferType::TRANSFER_SRC;
			stagingDescriptor.usage = BufferUsage::PERSISTENT;
			stagingDescriptor.memoryUsage = BufferMemoryUsage::CPU_ONLY;
		}

		Buffer stagingBuffer = allocate_buffer(vmaAllocator, stagingDescriptor, range.size);

		// Buffer data:
		if (data)
		{
			void* bufferData = nullptr;

			VkResult mapResult = vmaMapMemory(vmaAllocator, stagingBuffer.allocation, &bufferData);
			if (mapResult != VK_SUCCESS || !bufferData)
				TUR_LOG_CRITICAL("Failed to map memory from staging buffer");

			std::memcpy(bufferData, (byte*)data + range.offset, range.size);
			vmaUnmapMemory(vmaAllocator, stagingBuffer.allocation);
		}

		// Buffer copy:
		utils::submit_immediate_command(
			*rRHI,
			[&](vk::CommandBuffer commandBuffer)
			{
				vk::BufferCopy region;
				{
					region.dstOffset = 0;
					region.srcOffset = 0;
					region.size = range.size;
				}

				commandBuffer.copyBuffer(stagingBuffer.buffer, buffer.buffer, region);
			}
		);

		vmaDestroyBuffer(vmaAllocator, stagingBuffer.buffer, stagingBuffer.allocation);
		return mBuffers.add(buffer);
	}
	buffer_handle ResourceHandler::create_empty_buffer(const BufferDescriptor& descriptor, u32 size)
	{
		return mBuffers.add(allocate_buffer(rRHI->get_state().vmaAllocator, descriptor, size));
	}
	void ResourceHandler::update_buffer(buffer_handle bufferHandle, const void* data, Range range)
	{
		TUR_ASSERT(bufferHandle != invalid_handle, "Invalid buffer handle");

		TUR_ASSERT(data, "Invalid data pointer");
		TUR_ASSERT(range.size > 0, "Invalid range size");
		TUR_ASSERT(range.offset >= 0, "Invalid offset size");

		const Buffer& targetBuffer = mBuffers.get(bufferHandle);
		auto& vmaAllocator = rRHI->get_state().vmaAllocator;

		// Staging buffer:
		BufferDescriptor stagingDescriptor = {};
		{
			stagingDescriptor.memoryUsage = BufferMemoryUsage::CPU_ONLY;
			stagingDescriptor.type = BufferType::TRANSFER_SRC;
		}
		Buffer stagingBuffer = allocate_buffer(vmaAllocator, stagingDescriptor, range.size);

		{
			void* bufferData = nullptr;
			vmaMapMemory(vmaAllocator, stagingBuffer.allocation, &bufferData);
			std::memcpy(bufferData, (byte*)(data) + range.offset, range.size);
			vmaUnmapMemory(vmaAllocator, stagingBuffer.allocation);
		}

		// Buffer copy:
		utils::submit_immediate_command(
			*rRHI,
			[&](vk::CommandBuffer commandBuffer)
			{
				vk::BufferCopy region;
				{
					region.dstOffset = 0;
					region.srcOffset = 0;
					region.size = range.size;
				}

				commandBuffer.copyBuffer(stagingBuffer.buffer, targetBuffer.buffer, region);
			}
		);

		vmaDestroyBuffer(vmaAllocator, stagingBuffer.buffer, stagingBuffer.allocation);
	}
	void ResourceHandler::copy_buffer(
		buffer_handle source, buffer_handle destination, u32 size, u32 srcOffset, u32 dstOffset
	)
	{
		TUR_ASSERT(source != invalid_handle, "Invalid buffer_handle");

		utils::submit_immediate_command(
			*rRHI,
			[&](vk::CommandBuffer commandBuffer)
			{
				vk::BufferCopy region;
				{
					region.dstOffset = dstOffset;
					region.srcOffset = srcOffset;
					region.size = size;
				}

				commandBuffer.copyBuffer(mBuffers.get(source).buffer, mBuffers.get(destination).buffer, region);
			}
		);
	}
	void ResourceHandler::copy_buffer_to_texture(
		buffer_handle source, texture_handle destination, u32 width, u32 height, u32 depth
	)
	{
		TUR_ASSERT(source != invalid_handle, "Invalid buffer_handle");
		TUR_ASSERT(destination != invalid_handle, "Invalid texture_handle");

		const Buffer& buffer = mBuffers.get(source);
		const Texture& texture = mTextures.get(destination);

		utils::submit_immediate_command(
			*rRHI,
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
			}
		);
	}
	void ResourceHandler::destroy_buffer(buffer_handle bufferHandle)
	{
		TUR_ASSERT(bufferHandle != invalid_handle, "Invalid buffer_handle");
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
			// TODO: proper masks
			utils::transition_texture_layout_imm(
				*rRHI, texture, {.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal}
			);

		return handle;
	}
	texture_handle ResourceHandler::create_empty_texture(const TextureDescriptor& descriptor)
	{
		return mTextures.add(
			allocate_texture(rRHI->get_state().logicalDevice, rRHI->get_state().vmaAllocator, descriptor)
		);
	}
	void ResourceHandler::update_texture(texture_handle textureHandle, const TextureAsset& asset)
	{
		TUR_ASSERT(textureHandle != invalid_handle, "Invalid texture_handle");

		Texture& texture = mTextures.get(textureHandle);
		BufferDescriptor stagingDescriptor = {};
		{
			stagingDescriptor.memoryUsage = BufferMemoryUsage::CPU_ONLY;
			stagingDescriptor.type = BufferType::TRANSFER_SRC | BufferType::TRANSFER_DST;
		}

		buffer_handle stagingBufferHandle =
			create_buffer(stagingDescriptor, asset.data.data(), {.size = asset.data.size()});

		// TODO: prepare other image layouts
		{
			// TODO: proper masks
			utils::transition_texture_layout_imm(*rRHI, texture, {.newLayout = vk::ImageLayout::eTransferDstOptimal});

			copy_buffer_to_texture(stagingBufferHandle, textureHandle, asset.width, asset.height);

			// TODO: proper masks
			utils::transition_texture_layout_imm(
				*rRHI, texture, {.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal}
			);
		}

		destroy_buffer(stagingBufferHandle);
	}
	void ResourceHandler::destroy_texture(texture_handle textureHandle)
	{
		TUR_ASSERT(textureHandle != invalid_handle, "Invalid texture_handle");
		rRHI->get_deletion_queue().submit_texture(textureHandle);
	}
}