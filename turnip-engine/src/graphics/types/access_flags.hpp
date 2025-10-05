#pragma once
#include "common.hpp"

namespace tur
{
	enum class AccessFlags : u32
	{
		READ = 1 << 0,
		WRITE = 1 << 1,
		PERSISTENT = 1 << 2,
		COHERENT = 1 << 3
	};

	inline AccessFlags operator|(AccessFlags lhs, AccessFlags rhs)
	{
		return static_cast<AccessFlags>(static_cast<u32>(lhs) | static_cast<u32>(rhs));
	}
}