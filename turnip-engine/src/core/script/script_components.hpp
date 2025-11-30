#pragma once
#include <filesystem>
#include <sol/sol.hpp>

#include "common.hpp"
#include "scene/entity.hpp"

namespace tur
{
	struct ScriptComponent
	{
	public:
		ScriptComponent(const std::filesystem::path& filepath)
			: filepath(filepath)
		{
		}

	public:
		std::filesystem::path filepath;
		sol::environment environment;
	};
}