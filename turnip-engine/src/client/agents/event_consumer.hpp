#pragma once
#include "core/event/event.hpp"

namespace tur
{
	class IEventConsumer
	{
	public:
		virtual ~IEventConsumer() = default;

	public:
		virtual void on_event(Event& event) = 0;
	};
}