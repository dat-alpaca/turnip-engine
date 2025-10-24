#pragma once
#include "common.hpp"
#include <nlohmann/json.hpp>
#include <string>

namespace tur
{
	struct ScriptingInfo
	{
		std::string stubFilepath = "lib";
		NLOHMANN_DEFINE_TYPE_INTRUSIVE(ScriptingInfo, stubFilepath);
	};
}