#pragma once
#include "core/event/events.hpp"
#include <glm/glm.hpp>
#include <string>

#include "config/window_config.hpp"

namespace tur
{
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
	} && requires(T t)
	{
		{ t.get_time() }								-> std::same_as<f64>;
	};

	template <IsWindow T>
	class BaseWindow : public T
	{
	};
}