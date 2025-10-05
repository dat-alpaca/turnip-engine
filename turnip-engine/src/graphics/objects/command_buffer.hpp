#pragma once
#include "graphics/objects/render_target.hpp"
#include "graphics/types/clear_color.hpp"

namespace tur
{
	// clang-format off
	template <typename T, typename CommandBufferType>
	concept IsCommandBuffer = requires(T t, CommandBufferType commandBuffer, const ClearColor& clearColor)
	{
        { t.reset(commandBuffer) };
		{ t.set_clear_color(clearColor) };
	} &&
	requires(T t, render_target_handle renderTargetHandle)
	{
        { t.begin(renderTargetHandle) };
        { t.end() };
	};
	// clang-format on

	template <typename T, typename CommandBufferType>
	requires IsCommandBuffer<T, CommandBufferType>
	class BaseCommandBuffer : public T
	{
	};
}