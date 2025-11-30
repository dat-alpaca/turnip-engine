#pragma once
#include "common.hpp"
#include <nlohmann/json.hpp>

namespace tur
{
	struct PhysicsConfig
	{
		float gravityAcceleration = -9.8067f;
		float fixedTimeStep = 0.016666f;
		u32 substepCount = 4;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(PhysicsConfig, gravityAcceleration, fixedTimeStep, substepCount);
	};
}
