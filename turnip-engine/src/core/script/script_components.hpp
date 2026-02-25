#pragma once
#include <filesystem>
#include <sol/sol.hpp>
#include <sol/types.hpp>

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
		
		ScriptComponent() = default;
		~ScriptComponent()
		{
			instance = sol::nil;
		}

	public:
		std::filesystem::path filepath;
		
		FIELD_SET_INTERNALLY sol::table instance = sol::nil;
		FIELD_SET_INTERNALLY bool loaded = false;
	};
}