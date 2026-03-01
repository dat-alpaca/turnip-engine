#pragma once
#include "defines.hpp"
#include "graphics/objects/command_buffer.hpp"
#include "graphics/objects/image.hpp"
#include "rhi/opengl/objects/pipeline.hpp"
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
		void reset(command_buffer_handle commandBuffer);
		void execute_secondary_buffers(std::span<EmptyCommandBuffer>);
		void initialize_secondary();

	public:
		void begin(render_target_handle renderTarget = invalid_handle);
		void begin_rendering();
		void begin_compute();
		
		void end_rendering();
		void end();
		void blit([[maybe_unused]] image_handle imageHandle = invalid_handle);

	public:
		void set_clear_color(const ClearColor& clearColor, ClearFlags flags);
		void set_viewport(const Viewport& viewport);
		void set_scissor(const Extent2D& scissor);

	public:
		void bind_vertex_buffer(buffer_handle bufferHandle, u32 binding);
		void bind_index_buffer(buffer_handle bufferHandle, BufferIndexType indexType);
		void bind_pipeline(pipeline_handle pipelineHandle);
		void bind_descriptor_set(descriptor_set_handle setHandle);

	public:
		void draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance);
		void draw_indexed(u32 indexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance);

		void draw_indirect(buffer_handle bufferHandle, u64 offset, u32 drawCount, u32 stride);
		void draw_indexed_indirect(buffer_handle bufferHandle, u64 offset, u32 drawCount, u32 stride);

	public:
		void dispatch(u32 groupsX, u32 groupsY, u32 groupsZ);

	public:
		inline EmptyCommandBuffer get_buffer() const { return {}; }

	private:
		void clear();

	private:
		NON_OWNING RenderInterfaceGL* rRHI = nullptr;
		gl_handle mVAO = invalid_handle;
		BufferIndexType mBufferIndexType = BufferIndexType::UNSIGNED_INT;
		Pipeline mPipeline;

	private:
		ClearColor mColor;
		ClearFlags mClearFlags;
		u32 mRawClearFlags = 0;
		u32 mCurrentStride = 0;
	};
}

namespace tur
{
	using CommandBuffer = BaseCommandBuffer<gl::CommandBufferGL, gl::EmptyCommandBuffer>;
}