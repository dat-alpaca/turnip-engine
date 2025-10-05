#pragma once
#include "graphics/objects/command_buffer.hpp"
#include <vulkan/vulkan.hpp>

#include "rhi/opengl/opengl_common.hpp"

namespace tur::gl
{
	class RenderInterfaceGL;

	struct EmptyCommandBuffer
	{
	};

	class CommandBufferGL
	{
	public:
		CommandBufferGL(NON_OWNING RenderInterfaceGL* rhi);
		CommandBufferGL();

	public:
		void reset(EmptyCommandBuffer commandBuffer);

	public:
		void begin()
		{
			// TODO: create textures
		}
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
		NON_OWNING RenderInterfaceGL* rRHI = nullptr;
		gl_handle mVAO = invalid_handle;
	};
}

namespace tur
{
	using CommandBuffer = BaseCommandBuffer<gl::CommandBufferGL, EmptyCommandBuffer>;
}