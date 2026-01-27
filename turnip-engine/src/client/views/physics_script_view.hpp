#pragma once
#include "client/views/physics_view.hpp"
#include "view/view.hpp"
#include "client/views/script_view.hpp"

#include "entt/entity/fwd.hpp"
#include "event/physics_events/contact_event.hpp"
#include "event/subscriber.hpp"
#include "event/event.hpp"

#include "scene/scene.hpp"
#include "utils/script_utils.hpp"

#include "script/script_components.hpp"

namespace tur
{
	class PhysicsScriptView : public View
	{
	public:
		PhysicsScriptView(NON_OWNING PhysicsView* physics, NON_OWNING ScriptView* script)
		{
            TUR_ASSERT(physics, "Invalid physics system pointer");
            TUR_ASSERT(script, "Invalid script system pointer");

			rPhysicsView = physics;
			rScriptView = script;
		}

    public:
        void on_event(Event& event) override
        {
            Subscriber subscriber(event);
            subscriber.subscribe<SceneSwitchedEvent>([this](const SceneSwitchedEvent& event) -> bool {
				rScene = event.currentScene;
				return false;
			});

            subscriber.subscribe<OnContactEvent>([&](const OnContactEvent& contactEvent) -> bool {
                switch(contactEvent.type)
                {
                    case ContactType::BEGIN:
                        handle_begin_event(contactEvent.entityA, contactEvent.entityB);
                        break;

                     case ContactType::END:
                        handle_end_event(contactEvent.entityA, contactEvent.entityB);
                        break;

                     case ContactType::HIT:
                        handle_hit_event(contactEvent.entityA, contactEvent.entityB, contactEvent.speed);
                        break;
                }
                
                return false;
            });
        }

    private:
        void handle_begin_event(entt::entity entityA, entt::entity entityB)
        {
            Entity A { entityA, rScriptView->get_scene() };
            Entity B { entityB, rScriptView->get_scene() };
       
            if(A.has_component<ScriptComponent>())
            {
                auto& instance = A.get_component<ScriptComponent>().instance;
                auto function = get_function(instance, "on_contact_begin");
                if (function.valid())
                    function(instance, B);
            }

            if(B.has_component<ScriptComponent>())
            {
                auto& instance = B.get_component<ScriptComponent>().instance;
                auto function = get_function(instance, "on_contact_begin");
                if (function.valid())
                    function(instance, A);
            }
        }

        void handle_end_event(entt::entity entityA, entt::entity entityB)
        {
            Entity A { entityA, rScriptView->get_scene() };
            Entity B { entityB, rScriptView->get_scene() };
       
            if(A.has_component<ScriptComponent>())
            {
                auto& instance = A.get_component<ScriptComponent>().instance;
                auto function = get_function(instance, "on_contact_end");
                if (function.valid())
                    function(instance, B);
            }

            if(B.has_component<ScriptComponent>())
            {
                auto& instance = B.get_component<ScriptComponent>().instance;
                auto function = get_function(instance, "on_contact_end");
                if (function.valid())
                    function(instance, A);
            }
        }

        void handle_hit_event(entt::entity entityA, entt::entity entityB, float approachSpeed)
        {
            Entity A { entityA, rScriptView->get_scene() };
            Entity B { entityB, rScriptView->get_scene() };
       
            if(A.has_component<ScriptComponent>())
            {
                auto& instance = A.get_component<ScriptComponent>().instance;
                auto function = get_function(instance, "on_contact_hit");
                if(function.valid())
                    function(instance, B, approachSpeed);
            }

            if(B.has_component<ScriptComponent>())
            {
                auto& instance = B.get_component<ScriptComponent>().instance;
                auto function = get_function(instance, "on_contact_hit");
                if(function.valid())
                    function(instance, A, approachSpeed);
            }
        }

    private:
        NON_OWNING PhysicsView* rPhysicsView = nullptr; // TODO: remove
        NON_OWNING ScriptView* rScriptView = nullptr;
        NON_OWNING Scene* rScene = nullptr;
    };
}