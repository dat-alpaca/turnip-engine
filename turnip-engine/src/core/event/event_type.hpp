#pragma once
#include "common.hpp"

namespace tur
{
	enum class EventType : u64
	{
		UNKNOWN = 0,

		WINDOW_CLOSE,
		WINDOW_RESIZE,
		WINDOW_MOVED,
		WINDOW_MAXIMIZED,
		WINDOW_FOCUS_CHANGED,
		WINDOW_FRAMEBUFFER_RESIZE,
		WINDOW_CONTENT_SCALE,

		KEY_PRESSED,
		KEY_RELEASED,
		KEY_REPEAT,

		MOUSE_PRESSED,
		MOUSE_RELEASED,
		MOUSE_SCROLL,
		MOUSE_MOVE,

		SCENE_SWITCHED,

		ASSET_ON_LOAD_NEW_TEXTURE,

		AMOUNT
	};

	constexpr inline u64 NextAvailableEventID = static_cast<u64>(EventType::AMOUNT);
}