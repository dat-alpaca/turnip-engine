#pragma once
#include "event/event.hpp"
#include "scene/scene.hpp"

namespace tur
{
	struct SceneDeserializedEvent : public Event
	{
		DEFINE_EVENT(EventType::SCENE_DESERIALIZED);

	public:
		SceneDeserializedEvent(NON_OWNING Scene* scene)
			: scene(scene)
		{
		}

	public:
		NON_OWNING Scene* scene = nullptr;
	};
}