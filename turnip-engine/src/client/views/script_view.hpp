#pragma once
#include "view/view.hpp"
#include "entt/entity/fwd.hpp"
#include "event/event.hpp"
#include "event/window_events/window_framebuffer_event.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"
#include "script/script_handler.hpp"

#include "utils/script_utils.hpp"
#include "utils/time/time.hpp"

namespace tur
{
	class ScriptView : public View
	{
	public:
		ScriptView(NON_OWNING ScriptHandler* scriptHandler)
			: rScriptHandler(scriptHandler)
		{
		}

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

		void on_event(Event& event) override
		{
			Subscriber subscriber(event);
			subscriber.subscribe<WindowFramebufferEvent>(
				[&](const WindowFramebufferEvent& resizeEvent) -> bool
				{
					on_resize_event(resizeEvent.width, resizeEvent.height);
					return false;
				}
			);

			subscriber.subscribe<SceneSwitchedEvent>([this](const SceneSwitchedEvent& event) -> bool {
				rScene = event.currentScene;
				
				if(!rScriptHandler)
					return false;
				
				wake_up();
				return false;
			});
		}

		void on_fixed_update() override
		{
			auto view = rScene->get_registry().view<ScriptComponent>();
			for (auto [e, script] : view.each())
			{
				if(!has_function(script.instance, "on_fixed_update"))
					continue;

				call_expected_function(script.instance, "on_fixed_update", time);
			}
		}

		void on_update(const Time& time) override
		{
			auto view = rScene->get_registry().view<ScriptComponent>();
			for (auto [e, script] : view.each())
			{
				if(!has_function(script.instance, "on_update"))
					continue;

				call_expected_function(script.instance, "on_update", time);
			}
		}

		void on_post_update(const Time& time) override
		{
			auto view = rScene->get_registry().view<ScriptComponent>();
			for (auto [e, script] : view.each())
			{
				if(!has_function(script.instance, "on_post_update"))
					continue;
				
				call_expected_function(script.instance, "on_post_update", time);
			}
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
			
			instance["get_entity"] = [&](entt::entity entity)
			{
				return Entity{static_cast<entt::entity>(entity), rScene};
			};

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

	private:
		void on_resize_event(float width, float height)
		{
			if(!rScene)
				return;

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
		NON_OWNING Scene* rScene = nullptr;
		NON_OWNING ScriptHandler* rScriptHandler = nullptr;
	};
}