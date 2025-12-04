#pragma once
#include <sol/forward.hpp>
#include <sol/overload.hpp>
#include <string_view>

#include "physics/physics_components.hpp"
#include "scene/common_components.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"
#include "script/script_handler.hpp"

namespace tur
{
	class ScriptSystem
	{
	public:
		void initialize(NON_OWNING Scene* scene, NON_OWNING ScriptHandler* scriptHandler)
		{
			rScriptHandler = scriptHandler;
			set_scene(scene);
		}
		void set_scene(Scene* scene) { rScene = scene; }

	public:
		void wake_up()
		{
			initialize_registry();
			on_wake();
		}

	public:
		void on_wake()
		{
			for (auto [entity, script] : rScene->get_registry().view<ScriptComponent>().each())
				call_function(script.instance, "on_wake");
		}

		void on_update()
		{
			auto view = rScene->get_registry().view<ScriptComponent>();
			for (auto [e, script] : view.each())
				call_function(script.instance, "on_update");
		}

	private:
		void initialize_registry()
		{
			TUR_ASSERT(rScriptHandler, "Script Handler not bound.");
			TUR_ASSERT(rScene, "Scene not bound.");

			auto& lua = rScriptHandler->get_state();

			auto view = rScene->get_registry().view<ScriptComponent>();
			for (auto [e, script] : view.each())
			{
				auto scriptFile = lua.script_file(script.filepath);

				if (!scriptFile.valid())
				{
					sol::error err = scriptFile;
					TUR_LOG_ERROR("[Lua]: {}. File: {}", err.what(), script.filepath.string());
					continue;
				}

				sol::table instance = scriptFile;
				script.instance = instance["new"](instance);

				initialize_environment(script.instance, e);
			}
		}

		void initialize_environment(sol::table& instance, entt::entity entity)
		{
			instance["_id"] = static_cast<u32>(entity);

			instance["add_entity"] = [&]() { return rScene->add_entity(); };

			instance["add_component"] = [&, entity](const std::string& componentName)
			{ add_component(Entity{entity, rScene}, componentName); };
			
			instance["find_component"] = [&, entity](const std::string& componentName)
			{ return find_component(Entity{entity, rScene}, componentName); };
		}

		void call_function(sol::table& instance, std::string_view functionName)
		{
			auto function = instance[functionName];
			if (!function.valid() || function.get_type() != sol::type::function)
				TUR_LOG_ERROR("Failed to call function: {}", functionName);

			function(instance);
		}

	private:
		sol::object find_component(Entity entity, const std::string& componentName)
		{
			auto& lua = rScriptHandler->get_state();

			if (componentName == "uuid" && entity.has_component<UUIDComponent>())
				return sol::make_object(lua, &entity.get_component<UUIDComponent>().uuid);

			if (componentName == "name" && entity.has_component<NameComponent>())
				return sol::make_object(lua, &entity.get_component<NameComponent>().name);

			if (componentName == "transform" && entity.has_component<TransformComponent>())
				return sol::make_object(lua, &entity.get_component<TransformComponent>().transform);

			if (componentName == "audio_source" && entity.has_component<AudioSourceComponent>())
				return sol::make_object(lua, &entity.get_component<AudioSourceComponent>());

			if (componentName == "body2d" && entity.has_component<Body2DComponent>())
				return sol::make_object(lua, &entity.get_component<Body2DComponent>());

			return sol::nil;
		}

		void add_component(Entity entity, const std::string& componentName)
		{
			auto& lua = rScriptHandler->get_state();

			if (componentName == "transform" && !entity.has_component<TransformComponent>())
				entity.add_component<TransformComponent>();

			if (componentName == "audio_source" && !entity.has_component<AudioSourceComponent>())
				entity.add_component<AudioSourceComponent>();
			
			if (componentName == "body2d" && !entity.has_component<Body2DComponent>())
				entity.add_component<Body2DComponent>();
		}

	private:
		NON_OWNING ScriptHandler* rScriptHandler = nullptr;
		NON_OWNING Scene* rScene = nullptr;
	};
}