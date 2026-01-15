#pragma once
#include "core/event/event.hpp"
#include "scene/scene.hpp"

namespace tur
{
	struct SceneSwitchedEvent : public Event
	{
		DEFINE_EVENT(EventType::SCENE_SWITCHED);

	public:
		SceneSwitchedEvent(NON_OWNING Scene* currentScene) : currentScene(currentScene) {}

	public:
		Scene* currentScene = nullptr;
	};
}