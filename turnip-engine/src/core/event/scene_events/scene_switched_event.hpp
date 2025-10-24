#pragma once
#include "core/event/event.hpp"
#include "scene/scene.hpp"

namespace tur
{
	struct ViewSwitchedEvent : public Event
	{
		DEFINE_EVENT(EventType::SCENE_SWITCHED);

	public:
		ViewSwitchedEvent(NON_OWNING Scene* currentScene) : currentScene(currentScene) {}

	public:
		Scene* currentScene = nullptr;
	};
}