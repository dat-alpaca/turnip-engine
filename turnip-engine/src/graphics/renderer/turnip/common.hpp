#pragma once
#include "defines.hpp"

namespace tur
{
	struct DrawCommand
	{
		u32 count;
		u32 instanceCount;
		u32 firstIndex;
		u32 baseVertex;
		u32 baseInstance;
	};
}