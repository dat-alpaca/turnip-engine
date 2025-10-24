#pragma once
#include "event/event.hpp"
#include "memory/memory.hpp"
#include "platform/platform.hpp"
#include "scene/entity.hpp"
#include "script.hpp"

#include <filesystem>
#include <sol/sol.hpp>
#include <vector>

namespace tur
{
	class ScriptManager
	{
	public:
		static void initialize_entity_environment(Entity entity, sol::environment environment);
		static void initialize_core();
		static void initialize_input(Window& window);

	public:
		static inline sol::environment create_env() { return sol::environment(sLua, sol::create, sLua.globals()); }
		static inline sol::state& get_state() { return sLua; }

	private:
		static inline sol::state sLua;
	};
}