#pragma once
#include "graphics/objects/command_buffer.hpp"
#include "rhi/vulkan/objects/render_target.hpp"
#include <vulkan/vulkan.hpp>

#include "common.hpp"

namespace tur::vulkan
{
	class RenderInterfaceVulkan;

	class CommandBufferVulkan
	{
	public:
		CommandBufferVulkan(NON_OWNING RenderInterfaceVulkan* rhi);
		CommandBufferVulkan() = default;

	public:
		void reset(vk::CommandBuffer commandBuffer);
		void set_clear_color(const ClearColor& clearColor);

	public:
		void begin(render_target_handle renderTargetHandle = invalid_handle);
		void end();

		/*
				protected:
					void set_viewport(const Viewport& viewport);
					void set_scissor(const Rect2D& scissor);
					void clear(ClearFlags flags, const ClearValue& clearValue);

				protected:
					void bind_pipeline(pipeline_handle textureHandle);
					void bind_descriptor_set(descriptor_set_handle textureHandle);
					void bind_vertex_buffer(buffer_handle textureHandle, u32 binding, u32 stride);
					void bind_index_buffer(buffer_handle textureHandle, BufferIndexType type);

				protected:
					void draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance);
					void draw_indexed(u32 indexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance);

				private:
					void transition_image(vk::Image targetImage, vk::ImageLayout currentLayout, vk::ImageLayout
		   newLayout); void copy_image(vk::Image source, vk::Image target, vk::Extent2D sourceSize, vk::Extent2D
		   targetSize); */

	private:
		void copy_image(vk::Image source, vk::Image target, vk::Extent2D sourceSize, vk::Extent2D targetSize);
		void blit_onto_swapchain();

	private:
		NON_OWNING RenderInterfaceVulkan* rRHI = nullptr;
		vk::CommandBuffer mCommandBuffer;

	private:
		render_target_handle mUsedTarget;
		ClearColor mClearColor;
	};
}

namespace tur
{
	using CommandBuffer = BaseCommandBuffer<vulkan::CommandBufferVulkan, vk::CommandBuffer>;
}