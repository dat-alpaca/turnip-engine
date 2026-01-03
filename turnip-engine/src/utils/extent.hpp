#pragma once
#include "common.hpp"
#include "vulkan/vulkan.hpp"

namespace tur
{
	struct Extent2D
	{
		u32 x;
		u32 y;
		u32 width;
		u32 height;
	};

	struct Rect
	{
		float x, y, width, height;
	};
}