#pragma once
#include <string_view>

#include "scene/components.hpp"
#include "scene/scene.hpp"
#include "script/script_system.hpp"

namespace tur
{
	class ScriptSubsystem
	{
	public:
		void initialize(NON_OWNING Scene* scene, NON_OWNING ScriptSystem* scriptSystem)
		{
			rScriptSystem = scriptSystem;
			set_scene(scene);
		}
		void set_scene(Scene* scene) { rScene = scene; }

	public:
		void wake()
		{
			initialize_registry();
			on_wake();
		}

	public:
		void on_wake()
		{
			for (auto [entity, script] : rScene->get_registry().view<ScriptComponent>().each())
				call_function(script.environment, "on_wake");
		}

		void on_update()
		{
			auto view = rScene->get_registry().view<ScriptComponent>();
			for (auto [e, script] : view.each())
				call_function(script.environment, "on_update");
		}

	private:
		void initialize_registry()
		{
			TUR_ASSERT(rScriptSystem, "Script System not bound.");
			TUR_ASSERT(rScene, "Scene not bound.");

			auto& lua = rScriptSystem->get_state();

			auto view = rScene->get_registry().view<ScriptComponent>();
			for (auto [e, script] : view.each())
			{
				script.environment = rScriptSystem->create_environment();
				initialize_environment(script.environment, e);

				sol::protected_function_result result = lua.safe_script_file(script.filepath, script.environment);

				if (result.valid())
					continue;

				sol::error err = result;
				TUR_LOG_ERROR("[Lua]: {}", err.what());
			}
		}

		void initialize_environment(sol::environment& environment, entt::entity entity)
		{
			environment["_id"] = static_cast<u32>(entity);

			environment["find_component"] = [&, entity](const std::string& componentName) {
				return find_component(Entity{entity, rScene}, componentName);
			};
		}

		void call_function(sol::environment& environment, std::string_view functionName)
		{
			auto function = environment[functionName];
			if (!function.valid() || function.get_type() != sol::type::function)
				TUR_LOG_ERROR("Failed to call function: {}", functionName);

			function();
		}

	private:
		sol::object find_component(Entity entity, const std::string& componentName)
		{
			auto& lua = rScriptSystem->get_state();

			if (componentName == "uuid" && entity.has_component<UUIDComponent>())
				return sol::make_object(lua, &entity.get_component<UUIDComponent>().uuid);

			if (componentName == "name" && entity.has_component<NameComponent>())
				return sol::make_object(lua, &entity.get_component<NameComponent>().name);

			if (componentName == "transform" && entity.has_component<TransformComponent>())
				return sol::make_object(lua, &entity.get_component<TransformComponent>().transform);

			if (componentName == "audio_source" && entity.has_component<AudioSourceComponent>())
				return sol::make_object(lua, &entity.get_component<AudioSourceComponent>());

			return sol::nil;
		}

	private:
		NON_OWNING ScriptSystem* rScriptSystem = nullptr;
		NON_OWNING Scene* rScene = nullptr;
	};
}