#pragma once
#include <filesystem>
#include <sol/sol.hpp>

#include "common.hpp"
#include "scene/entity.hpp"
#include "script.hpp"

namespace tur
{
	struct ScriptComponent
	{
	public:
		ScriptComponent(Entity entity, const std::filesystem::path& filepath);
		ScriptComponent() = default;

	public:
		Script script;
	};
}