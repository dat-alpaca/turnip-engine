#pragma once
#include "common.hpp"
#include <nlohmann/json.hpp>
#include <string>

namespace tur
{
	struct WorkerConfig
	{
		u64 threadAmount = 4;
		NLOHMANN_DEFINE_TYPE_INTRUSIVE(WorkerConfig, threadAmount);
	};
}