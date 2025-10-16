#pragma once
#include "graphics/objects/buffer.hpp"
#include "graphics/objects/descriptor.hpp"
#include "graphics/objects/pipeline.hpp"
#include "graphics/objects/render_target.hpp"

#include "graphics/types/clear_color.hpp"
#include "graphics/types/extent.hpp"
#include "graphics/types/viewport.hpp"

namespace tur
{
	// clang-format off
	template <typename T, typename CommandBufferType>
	concept IsCommandBuffer = requires(T t, CommandBufferType commandBuffer)
	{
        { t.reset(commandBuffer) };
	} &&
	
	requires(T t, render_target_handle renderTargetHandle)
	{
		{ t.begin(renderTargetHandle) };
        { t.end() };
	} &&
	
	requires(T t, const ClearColor& clearColor, ClearFlags flags)
	{
		{ t.clear(clearColor, flags) };
	} &&

	requires(T t, const Viewport& viewport, const Extent2D& scissor)
	{
        { t.set_viewport(viewport) };
        { t.set_scissor(scissor) };
	} &&

	requires(T t, buffer_handle buffer, u32 binding, BufferIndexType indexType)
	{
        { t.bind_vertex_buffer(buffer, binding) };
        { t.bind_index_buffer(buffer, indexType) };
	} &&
	requires(T t, pipeline_handle pipelineHandle)
	{
        { t.bind_pipeline(pipelineHandle) };
	} &&
	requires(T t, descriptor_set_handle setHandle)
	{
        { t.bind_descriptor_set(setHandle) };
	} &&

	requires(T t, u32 vertexCount, u32 indexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance)
	{
        { t.draw(vertexCount, instanceCount, firstVertex, firstInstance) };
        { t.draw_indexed(indexCount, instanceCount, firstVertex, firstInstance) };
	};

	// clang-format on

	template <typename T, typename CommandBufferType>
	requires IsCommandBuffer<T, CommandBufferType>
	class BaseCommandBuffer : public T
	{
	};
}