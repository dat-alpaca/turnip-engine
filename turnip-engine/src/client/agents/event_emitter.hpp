#pragma once
#include "event/events.hpp"

namespace tur
{
	class IEventEmitter
	{
	public:
		void set_event_callback(EventCallback&& callback) { mCallback = callback; }

	protected:
		EventCallback callback;
	};
}