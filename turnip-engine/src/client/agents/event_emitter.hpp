#pragma once
#include "event/events.hpp"

namespace tur
{
	class IEventEmitter
	{
	public:
		void set_event_callback(const EventCallback& callback) { this->callback = callback; }

	protected:
		EventCallback callback;
	};
}