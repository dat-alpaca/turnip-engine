#pragma once
#include "core/event/event.hpp"

namespace tur
{
	enum class MouseButton
	{
		MOUSE_BUTTON_UNKNOWN = 0,
		MOUSE_BUTTON_1 = 1,
		MOUSE_BUTTON_2 = 2,
		MOUSE_BUTTON_3 = 3,
		MOUSE_BUTTON_4 = 4,
		MOUSE_BUTTON_5 = 5,
		MOUSE_BUTTON_6 = 6,
		MOUSE_BUTTON_7 = 7,
		MOUSE_BUTTON_8 = 8,
		MOUSE_BUTTON_LEFT = MOUSE_BUTTON_1,
		MOUSE_BUTTON_RIGHT = MOUSE_BUTTON_2
	};

	inline constexpr const char* get_mouse_button_name(MouseButton button)
	{
		switch (button)
		{
			case MouseButton::MOUSE_BUTTON_UNKNOWN:
				return "MOUSE_BUTTON_UNKNOWN";

			case MouseButton::MOUSE_BUTTON_1:
				return "MOUSE_BUTTON_1";

			case MouseButton::MOUSE_BUTTON_2:
				return "MOUSE_BUTTON_2";

			case MouseButton::MOUSE_BUTTON_3:
				return "MOUSE_BUTTON_3";

			case MouseButton::MOUSE_BUTTON_4:
				return "MOUSE_BUTTON_4";

			case MouseButton::MOUSE_BUTTON_5:
				return "MOUSE_BUTTON_5";

			case MouseButton::MOUSE_BUTTON_6:
				return "MOUSE_BUTTON_6";

			case MouseButton::MOUSE_BUTTON_7:
				return "MOUSE_BUTTON_7";

			case MouseButton::MOUSE_BUTTON_8:
				return "MOUSE_BUTTON_8";
		}
	}
}