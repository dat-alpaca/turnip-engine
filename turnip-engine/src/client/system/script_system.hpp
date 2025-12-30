#pragma once
#include <sol/forward.hpp>
#include <sol/overload.hpp>
#include <sol/types.hpp>
#include <string_view>

#include "client/system/camera_system.hpp"
#include "event/window_events/window_framebuffer_event.hpp"
#include "graphics/components.hpp"
#include "physics/physics_components.hpp"
#include "scene/common_components.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"
#include "script/script_handler.hpp"

#include "utils/time/time.hpp"

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
				call_expected_function(script.instance, "on_wake");
		}

		void on_fixed_update()
		{
			auto view = rScene->get_registry().view<ScriptComponent>();
			for (auto [e, script] : view.each())
			{
				if(!has_function(script.instance, "on_fixed_update"))
					continue;

				call_expected_function(script.instance, "on_fixed_update", time);
			}
		}

		void on_update(const Time& time)
		{
			auto view = rScene->get_registry().view<ScriptComponent>();
			for (auto [e, script] : view.each())
			{
				if(!has_function(script.instance, "on_update"))
					continue;

				call_expected_function(script.instance, "on_update", time);
			}
		}

		void on_post_update(const Time& time)
		{
			auto view = rScene->get_registry().view<ScriptComponent>();
			for (auto [e, script] : view.each())
			{
				if(!has_function(script.instance, "on_post_update"))
					continue;
				
				call_expected_function(script.instance, "on_post_update", time);
			}
		}

		sol::function get_function(sol::table& instance, std::string_view functionName)
		{
			auto function = instance[functionName];
			if (!function.valid() || function.get_type() != sol::type::function)
				return {};

			return function;
		}

	public:
		void on_event(Event& event)
		{
			Subscriber subscriber(event);
			subscriber.subscribe<WindowFramebufferEvent>(
				[&](const WindowFramebufferEvent& resizeEvent) -> bool
				{
					on_resize_event(resizeEvent.width, resizeEvent.height);
					return false;
				}
			);
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
			instance["_entity"] = Entity { entity, rScene };

			instance["add_entity"] = [&]() { return rScene->add_entity(); };
			
			// Aliases:
			instance["add_component"] = [&, entity](const std::string& componentName)
			{ 
				return rScriptHandler->add_component(
					Entity { entity, rScene }, 
					componentName
				);
			};
			
			instance["find_component"] = [&, entity](const std::string& componentName)
			{ 
				return rScriptHandler->find_component(
					Entity { entity, rScene }, 
					componentName
				);
			};
		}

		template<typename... Args>
		void call_expected_function(sol::table& instance, std::string_view functionName, Args&&... args)
		{
			auto function = instance[functionName];

			if (!function.valid() || function.get_type() != sol::type::function)
				TUR_LOG_ERROR("Failed to call function: {}", functionName);

			function(instance, args...);
		}

		bool has_function(sol::table& instance, std::string_view functionName)
		{
			if(instance == sol::nil)
				return false;

			return instance[functionName].valid();
		}

	private:
		void on_resize_event(float width, float height)
		{
			for (auto [entity, script] : rScene->get_registry().view<ScriptComponent>().each())
			{
				if(!has_function(script.instance, "on_window_resize"))
					continue;

				call_expected_function(script.instance, "on_window_resize", width, height);
			}
		}
		
	public:
		Scene* get_scene() const { return rScene; }

	private:
		NON_OWNING ScriptHandler* rScriptHandler = nullptr;
		NON_OWNING Scene* rScene = nullptr;
	};
}