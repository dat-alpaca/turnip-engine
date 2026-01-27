#pragma once
#include "core/event/event.hpp"
#include "memory/memory.hpp"
#include "graphics/camera.hpp"

namespace tur
{
	struct CameraSwitchedEvent : public Event
	{
		DEFINE_EVENT(EventType::CAMERA_SWITCHED);

	public:
		CameraSwitchedEvent(NON_OWNING Camera* camera) 
			: camera(camera) { }

	public:
		NON_OWNING Camera* camera = nullptr;
	};
}