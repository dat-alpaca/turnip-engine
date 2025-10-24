#include "vulkan_deletion_queue.hpp"
#include "rhi/vulkan/vulkan_render_interface.hpp"

namespace tur::vulkan
{
	void DeletionQueue::initialize(NON_OWNING RenderInterfaceVulkan* rhi)
	{
		rRHI = rhi;
	}

	void DeletionQueue::submit_general(std::function<void()>&& function)
	{
		mAdditionalDeletionQueue.push_back(std::move(function));
	}
	void DeletionQueue::submit_buffer(buffer_handle bufferHandle)
	{
		mBufferHandles.push_back(bufferHandle);
	}
	void DeletionQueue::submit_texture(texture_handle textureHandle)
	{
		mTextureHandles.push_back(textureHandle);
	}
	void DeletionQueue::submit_pipeline(pipeline_handle pipelineHandle)
	{
		mPipelineHandles.push_back(pipelineHandle);
	}
	void DeletionQueue::submit_descriptor_set_layout(descriptor_set_layout_handle setLayoutHandle)
	{
		mSetLayoutHandles.push_back(setLayoutHandle);
	}

	void DeletionQueue::flush()
	{
		auto& buffers = rRHI->get_resource_handler().get_buffers();
		auto& vmaAllocator = rRHI->get_state().vmaAllocator;
		auto& device = rRHI->get_state().logicalDevice;

		for (const auto& deletionFunction : mAdditionalDeletionQueue)
			deletionFunction();

		// Buffers:
		for (auto& bufferHandle : mBufferHandles)
		{
			if (bufferHandle == invalid_handle)
				continue;

			const Buffer& buffer = buffers.get(bufferHandle);
			vmaDestroyBuffer(vmaAllocator, buffer.buffer, buffer.allocation);

			buffers.remove(bufferHandle);
			bufferHandle = invalid_handle;
		}
		mBufferHandles.clear();

		// Textures:
		auto& textures = rRHI->get_resource_handler().get_textures();
		for (auto& textureHandle : mTextureHandles)
		{
			if (textureHandle == invalid_handle)
				continue;

			const Texture& texture = textures.get(textureHandle);
			vmaDestroyImage(vmaAllocator, texture.image, texture.allocation);
			device.destroyImageView(texture.imageView);
			device.destroySampler(texture.sampler);

			textures.remove(textureHandle);
			textureHandle = invalid_handle;
		}
		mTextureHandles.clear();

		// Pipelines:
		auto& pipelines = rRHI->get_resource_handler().get_pipelines();
		for (auto& pipelineHandle : mPipelineHandles)
		{
			if (pipelineHandle == invalid_handle)
				continue;

			Pipeline& pipeline = pipelines.get(pipelineHandle);
			device.destroyPipelineLayout(pipeline.layout);
			device.destroyPipeline(pipeline.pipeline);
		}
		mPipelineHandles.clear();

		// Set Layouts:
		auto& setLayouts = rRHI->get_resource_handler().get_set_layouts();
		for (auto& setLayout : mSetLayoutHandles)
		{
			if (setLayout == invalid_handle)
				continue;

			device.destroyDescriptorSetLayout(setLayouts.get(setLayout).layout);
			device.destroyDescriptorPool(setLayouts.get(setLayout).pool);
		}
		mSetLayoutHandles.clear();
	}
}
