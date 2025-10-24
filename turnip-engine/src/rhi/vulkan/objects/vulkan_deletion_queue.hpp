#pragma once
#include <functional>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "common.hpp"
#include "core/data-structures/free_list.hpp"
#include "rhi/vulkan/objects/buffer.hpp"
#include "rhi/vulkan/objects/descriptor.hpp"
#include "rhi/vulkan/objects/pipeline.hpp"
#include "rhi/vulkan/objects/texture.hpp"

namespace tur::vulkan
{
	class GraphicsDeviceVulkan;
}

namespace tur::vulkan
{
	class RenderInterfaceVulkan;

	class DeletionQueue
	{
	public:
		void initialize(NON_OWNING RenderInterfaceVulkan* rhi);

	public:
		void submit_general(std::function<void()>&& function);
		void submit_buffer(buffer_handle bufferHandle);
		void submit_texture(texture_handle textureHandle);
		void submit_pipeline(pipeline_handle pipelineHandle);
		void submit_descriptor_set_layout(descriptor_set_layout_handle setLayoutHandle);

		void flush();

	private:
		NON_OWNING RenderInterfaceVulkan* rRHI = nullptr;

	private:
		std::vector<buffer_handle> mBufferHandles;
		std::vector<texture_handle> mTextureHandles;
		std::vector<pipeline_handle> mPipelineHandles;
		std::vector<descriptor_set_layout_handle> mSetLayoutHandles;
		std::vector<std::function<void()>> mAdditionalDeletionQueue;
	};
}