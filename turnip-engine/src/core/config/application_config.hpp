#pragma once
#include <nlohmann/json.hpp>
#include <string>

#include "common.hpp"

namespace tur
{
	struct ApplicationSpecification
	{
		std::string applicationName = "Default Application";
		u32 major = 1, minor = 0, patch = 0, variant = 0;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(ApplicationSpecification, applicationName, major, minor, patch, variant);
	};
}