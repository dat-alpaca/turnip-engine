#pragma once
#include "client/agents/event_consumer.hpp"
#include "client/system/physics_system.hpp"
#include "client/system/script_system.hpp"

#include "entt/entity/fwd.hpp"
#include "event/physics_events/contact_event.hpp"
#include "event/subscriber.hpp"
#include "physics/physics_components.hpp"
#include "physics/physics_handler.hpp"
#include "event/event.hpp"

#include "scene/components.hpp"
#include "scene/scene.hpp"

#include "script/script_components.hpp"
#include "script/script_handler.hpp"

namespace tur
{
	class PhysicsScriptAgent : public IEventConsumer
	{
	public:
		void initialize(NON_OWNING Scene* scene, NON_OWNING PhysicsSystem* physics, NON_OWNING ScriptSystem* script)
		{
            TUR_ASSERT(physics, "Invalid physics system pointer");
            TUR_ASSERT(script, "Invalid script system pointer");

			rPhysicsSystem = physics;
			rScriptSystem = script;

			set_scene(scene);
		}
		void set_scene(NON_OWNING Scene* scene) { rScene = scene; }

    public:
        void on_wake()
        {
            auto view = rScene->get_registry().view<const Body2DComponent>();
			for (auto [entity, body] : view.each())
			{
                // TODO: Update body properties
                b2Body_SetType(body.bodyID, body.bodyDef.type);
            }
        }

        void on_event(Event& event)
        {
            Subscriber subscriber(event);
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
            Entity A { entityA, rScriptSystem->get_scene() };
            Entity B { entityB, rScriptSystem->get_scene() };
       
            if(A.has_component<ScriptComponent>())
            {
                auto& instance = A.get_component<ScriptComponent>().instance;
                auto function = rScriptSystem->get_function(instance, "on_contact_begin");
                if (function.valid())
                    function(instance, B);
            }

            if(B.has_component<ScriptComponent>())
            {
                auto& instance = B.get_component<ScriptComponent>().instance;
                auto function = rScriptSystem->get_function(instance, "on_contact_begin");
                if (function.valid())
                    function(instance, A);
            }
        }

        void handle_end_event(entt::entity entityA, entt::entity entityB)
        {
            Entity A { entityA, rScriptSystem->get_scene() };
            Entity B { entityB, rScriptSystem->get_scene() };
       
            if(A.has_component<ScriptComponent>())
            {
                auto& instance = A.get_component<ScriptComponent>().instance;
                auto function = rScriptSystem->get_function(instance, "on_contact_end");
                if (function.valid())
                    function(instance, B);
            }

            if(B.has_component<ScriptComponent>())
            {
                auto& instance = B.get_component<ScriptComponent>().instance;
                auto function = rScriptSystem->get_function(instance, "on_contact_end");
                if (function.valid())
                    function(instance, A);
            }
        }

        void handle_hit_event(entt::entity entityA, entt::entity entityB, float approachSpeed)
        {
            Entity A { entityA, rScriptSystem->get_scene() };
            Entity B { entityB, rScriptSystem->get_scene() };
       
            if(A.has_component<ScriptComponent>())
            {
                auto& instance = A.get_component<ScriptComponent>().instance;
                auto function = rScriptSystem->get_function(instance, "on_contact_hit");
                if(function.valid())
                    function(instance, B, approachSpeed);
            }

            if(B.has_component<ScriptComponent>())
            {
                auto& instance = B.get_component<ScriptComponent>().instance;
                auto function = rScriptSystem->get_function(instance, "on_contact_hit");
                if(function.valid())
                    function(instance, A, approachSpeed);
            }
        }

    private:
        NON_OWNING PhysicsSystem* rPhysicsSystem = nullptr; // TODO: remove
        NON_OWNING ScriptSystem* rScriptSystem = nullptr;
        NON_OWNING Scene* rScene = nullptr;
    };
}