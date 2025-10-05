#pragma once
#include "event_type.hpp"

#define DEFINE_EVENT(ID)                                                                                               \
public:                                                                                                                \
	static inline tur::event_id get_id()                                                                               \
	{                                                                                                                  \
		return static_cast<tur::event_id>(ID);                                                                         \
	}                                                                                                                  \
	tur::event_id id() const override                                                                                  \
	{                                                                                                                  \
		return static_cast<tur::event_id>(ID);                                                                         \
	};

namespace tur
{
	using event_id = u64;

	class Event
	{
	public:
		virtual ~Event() = default;

	public:
		virtual event_id id() const { return static_cast<event_id>(EventType::UNKNOWN); }

	public:
		bool handled = false;
	};

	using EventCallback = std::function<void(Event&)>;
}