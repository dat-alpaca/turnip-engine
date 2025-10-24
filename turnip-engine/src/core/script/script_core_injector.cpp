#include "scene/common_components.hpp"
#include "scene/entity.hpp"
#include "script_system.hpp"

namespace
{
	inline void script_script_core_logging(sol::state& luaState)
	{
		// clang-format off
		luaState["Log"] = luaState.create_table();
		luaState["Log"]["critical"] 	= [](const std::string& message) { TUR_LOG_CRITICAL(message); };
		luaState["Log"]["error"] 		= [](const std::string& message) { TUR_LOG_ERROR(message); };
		luaState["Log"]["warn"] 		= [](const std::string& message) { TUR_LOG_WARN(message); };
		luaState["Log"]["debug"] 		= [](const std::string& message) { TUR_LOG_DEBUG(message); };
		luaState["Log"]["trace"] 		= [](const std::string& message) { TUR_LOG_TRACE(message); };
		luaState["Log"]["info"] 		= [](const std::string& message) { TUR_LOG_INFO(message); };
		// clang-format on
	}

	inline void setup_script_core_components(sol::state& luaState)
	{
		using namespace tur;
		auto& lua = luaState;

		// UUID:
		lua.new_usertype<UUID>("UUID", "uuid", &UUID::uuid);
		lua.new_usertype<UUIDComponent>("UUIDComponent", "uuid", &UUIDComponent::uuid);

		// Name:
		lua.new_usertype<NameComponent>("NameComponent", "name", &NameComponent::name);

		// Transform:
		{
			// vec2:
			lua.new_usertype<glm::vec2>("vec2", "x", &glm::vec2::x, "y", &glm::vec2::y);

			// vec3:
			lua.new_usertype<glm::vec3>("vec3", "x", &glm::vec3::x, "y", &glm::vec3::y, "z", &glm::vec3::z);

			// vec4:
			lua.new_usertype<glm::vec4>(
				"vec4", "x", &glm::vec4::x, "y", &glm::vec4::y, "z", &glm::vec4::z, "w", &glm::vec4::w
			);
		}
		lua.new_usertype<Transform>(
			"Transform", "position", &Transform::position, "rotation", &Transform::rotation, "scale", &Transform::scale
		);

		lua.new_usertype<TransformComponent>("TransformComponent", "transform", &TransformComponent::transform);
	}
}

// Methods:
namespace tur
{
	inline sol::object script_find_component(
		const sol::state& luaState, Scene* scene, entt::entity entity, const std::string& componentName
	)
	{
		const auto& registry = scene->get_registry();
		const auto& lua = luaState;

		// TODO: consider caching

		if (componentName == "uuid" && registry.any_of<UUIDComponent>(entity))
			return sol::make_object(lua, &registry.get<UUIDComponent>(entity));

		if (componentName == "name" && registry.any_of<NameComponent>(entity))
			return sol::make_object(lua, &registry.get<NameComponent>(entity));

		if (componentName == "transform" && registry.any_of<TransformComponent>(entity))
			return sol::make_object(lua, &registry.get<TransformComponent>(entity));

		return sol::nil;
	}
}

namespace tur
{
	void ScriptManager::initialize_core()
	{
		sLua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::package, sol::lib::table);

		script_script_core_logging(sLua);
		setup_script_core_components(sLua);
	}

	void ScriptManager::initialize_entity_environment(Entity entity, sol::environment environment)
	{
		environment["find_component"] = [&, entity](const std::string& componentName)
		{ return script_find_component(sLua, entity.get_scene(), entity.get_handle(), componentName); };
	}
}