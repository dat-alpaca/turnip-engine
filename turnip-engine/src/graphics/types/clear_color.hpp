#pragma once
#include "utils/color.hpp"
#include <glm/glm.hpp>
#include <numeric>

namespace tur
{
	enum class ClearFlags : u32
	{
		COLOR = 1 << 0,
		DEPTH = 1 << 1,
		STENCIL = 1 << 1,
	};

	struct ClearColor
	{
		Color color = color::White;
		float depth = 1.0f;
		u32 stencil = 0;
	};
}