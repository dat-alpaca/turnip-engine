#pragma once
#include "common.hpp"

namespace tur
{
	struct EngineSpecification
	{
		u32 major = TUR_ENGINE_VERSION_MAJOR;
		u32 minor = TUR_ENGINE_VERSION_MINOR;
		u32 patch = 0;
		u32 variant = 0;
	};
}