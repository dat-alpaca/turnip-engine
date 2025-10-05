#pragma once
#include "common.hpp"

namespace tur
{
	struct Viewport
	{
		float x = 0;
		float y = 0;

		float width = 1.0f;
		float height = 1.0f;

		float minDepth = 0.f;
		float maxDepth = 1.f;
	};
}