#pragma once
#include "event.hpp"

namespace tur
{
	class Subscriber
	{
	public:
		Subscriber(Event& event) : mEvent(event) {}

	public:
		template <typename ImplementedEvent>
		void subscribe(std::function<bool(ImplementedEvent)> callback)
		{
			if (mEvent.handled)
				return;

			if (mEvent.id() != ImplementedEvent::get_id())
				return;

			ImplementedEvent* implementedEvent = static_cast<ImplementedEvent*>(&mEvent);
			implementedEvent->handled = implementedEvent->handled || callback(*implementedEvent);
		}

	private:
		Event& mEvent;
	};
}