#pragma once
#include "event/event.hpp"
#include "scene/scene.hpp"
#include "client/project/project.hpp"

namespace tur
{
	struct ScenePreDeserializationEvent : public Event
	{
		DEFINE_EVENT(EventType::SCENE_PRE_DESERIALIZED);

	public:
		ScenePreDeserializationEvent(NON_OWNING Scene* scene)
			: scene(scene)
		{
		}

	public:
		NON_OWNING Scene* scene = nullptr;
	};

	struct SceneDeserializedEvent : public Event
	{
		DEFINE_EVENT(EventType::SCENE_DESERIALIZED);

	public:
		SceneDeserializedEvent(NON_OWNING Scene* scene, const Project& project)
			: scene(scene)
			, project(project)
		{
		}

	public:
		NON_OWNING Scene* scene = nullptr;
		Project project;
	};
}