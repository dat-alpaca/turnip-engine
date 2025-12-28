#pragma once
#include "event/event.hpp"
#include "scene/scene.hpp"
#include "client/project/project.hpp"

namespace tur
{
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