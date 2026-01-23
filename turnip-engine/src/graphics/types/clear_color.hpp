#pragma once
#include "utils/color.hpp"
#include <glm/glm.hpp>

namespace tur
{
	enum class ClearFlags : u32
	{
		COLOR = 1 << 0,
		DEPTH = 1 << 1,
		STENCIL = 1 << 2,
		ALL = COLOR | DEPTH | STENCIL
	};

	inline ClearFlags operator|(ClearFlags lhs, ClearFlags rhs)
	{
		return static_cast<ClearFlags>(static_cast<u32>(lhs) | static_cast<u32>(rhs));
	}

	struct ClearColor
	{
	public:
		ClearColor() = default;

		ClearColor(Color color)
			: color(color) {}

	public:
		Color color = color::Black;
		float depth = 1.0f;
		u32 stencil = 0;
	};
}