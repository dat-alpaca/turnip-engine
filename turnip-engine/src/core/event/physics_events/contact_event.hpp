#pragma once
#include "core/event/event.hpp"
#include "entt/entity/fwd.hpp"
#include "scene/scene.hpp"

namespace tur
{
    enum class ContactType
    {
        BEGIN,
        END,
        HIT
    };

	struct OnContactEvent : public Event
	{
		DEFINE_EVENT(EventType::PHYSICS_CONTACT_EVENT);

	public:
		OnContactEvent(ContactType type, entt::entity entityA, entt::entity entityB, float speed = 0.0f) 
            : type(type)
            , entityA(entityA)
            , entityB(entityB)
            , speed(speed)
        {}

	public:
		ContactType type;
        entt::entity entityA;
        entt::entity entityB;
        float speed = 0.0f;
	};
}