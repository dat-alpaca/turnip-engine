#pragma once
#include "core/event/event.hpp"
#include "memory/memory.hpp"
#include "scene/scene.hpp"

namespace tur
{
	struct SceneSwitchedEvent : public Event
	{
		DEFINE_EVENT(EventType::SCENE_SWITCHED);

	public:
		SceneSwitchedEvent(NON_OWNING Scene* currentScene, NON_OWNING Scene* previousScene) 
			: currentScene(currentScene)
			, previousScene(previousScene) {}

	public:
		NON_OWNING Scene* currentScene = nullptr;
		NON_OWNING Scene* previousScene = nullptr;
	};
}