#pragma once
#include "core/event/events.hpp"
#include <glm/glm.hpp>
#include <string>

namespace
{
	static std::string default_window_title()
	{
		return "TurnipEngine v" + std::to_string(TUR_ENGINE_VERSION_MAJOR) + "."
			   + std::to_string(TUR_ENGINE_VERSION_MINOR);
	}
}

namespace tur
{
	struct WindowProperties
	{
		enum Position : int
		{
			DEFAULT = -1
		};

	public:
		std::string title = default_window_title();
		glm::uvec2 dimensions = glm::vec2(640, 480);
		glm::uvec2 position = glm::vec2(Position::DEFAULT, Position::DEFAULT);
		glm::uvec2 minSize = glm::vec2(0);
		glm::uvec2 maxSize = dimensions;
		bool startsMaximized = false;
	};

	// clang-format off
	template <typename T>
	concept IsWindow = requires(T t, EventCallback& callback, const WindowProperties& properties)
	{
		{ t.create(properties) }						-> std::same_as<void>;

		{ t.set_event_callback(callback) }   			-> std::same_as<void>;
		{ t.set_properties(properties) }				-> std::same_as<void>;
		{ t.poll_events() }								-> std::same_as<void>;
		{ t.wait_events() }								-> std::same_as<void>;

		{ t.maximize() }								-> std::same_as<void>;
		{ t.show() }									-> std::same_as<void>;
		{ t.hide() }									-> std::same_as<void>;
		{ t.is_open() }									-> std::same_as<bool>;
		
	} && requires(T t)
	{
		{ t.get_dimensions() }							-> std::same_as<glm::vec2>;
	} && requires(T t, MouseButton mouseButton, Key key)
	{
		{ t.get_mouse_pressed(mouseButton) }			-> std::same_as<bool>;
		{ t.get_key_pressed(key) }						-> std::same_as<bool>;
		{ t.get_mouse_position() }						-> std::same_as<glm::vec2>;
	};
	// clang-format on

	template <IsWindow T>
	class BaseWindow : public T
	{
	};
}