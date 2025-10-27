#pragma once
#include "graphics/objects/command_buffer.hpp"
#include "rhi/opengl/objects/pipeline.hpp"
#include "rhi/opengl/opengl_common.hpp"

namespace tur::noop
{
	class RenderInterfaceNOOP;

	struct EmptyCommandBuffer
	{
	};

	class CommandBufferNOOP
	{
	public:
		CommandBufferNOOP(NON_OWNING RenderInterfaceNOOP* rhi) {}
		CommandBufferNOOP() {}

	public:
		void reset(EmptyCommandBuffer commandBuffer) {}

	public:
		void begin(render_target_handle renderTarget = invalid_handle) {}
		void end() {}

	public:
		void set_viewport(const Viewport& viewport) {}
		void set_scissor(const Extent2D& scissor) {}
		void set_clear_color(const ClearColor& clearColor, ClearFlags flags) {}

	public:
		void bind_vertex_buffer(buffer_handle bufferHandle, u32 binding) {}
		void bind_index_buffer(buffer_handle bufferHandle, BufferIndexType indexType) {}
		void bind_pipeline(pipeline_handle pipelineHandle) {}
		void bind_descriptor_set(descriptor_set_handle setHandle) {}

	public:
		void draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance) {}
		void draw_indexed(u32 indexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance) {}

	private:
		void clear() {}

	private:
		NON_OWNING RenderInterfaceNOOP* rRHI = nullptr;
	};
}

namespace tur
{
	using CommandBuffer = BaseCommandBuffer<noop::CommandBufferNOOP, noop::EmptyCommandBuffer>;
}