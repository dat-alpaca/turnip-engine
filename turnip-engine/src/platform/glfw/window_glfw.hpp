#pragma once
#include "windowing/window.hpp"
#include <GLFW/glfw3.h>

namespace tur::platform
{
	class WindowDeleter
	{
	public:
		void operator()(GLFWwindow* window) const { glfwDestroyWindow(window); }
	};

	class WindowGLFW
	{
	public:
		void create(const WindowProperties& properties);
		void destroy();

	public:
		void set_event_callback(EventCallback& callback);
		void set_properties(const WindowProperties& properties);
		void poll_events();
		void wait_events();

	public:
		void maximize();
		void show();
		void hide();
		bool is_open() const;

	public:
		glm::vec2 get_dimensions();

	public:
		bool get_mouse_pressed(MouseButton mouseButton);
		bool get_key_pressed(Key key);
		glm::vec2 get_mouse_position();

	public:
		inline GLFWwindow* get_window() const { return mWindow.get(); }

		struct Data
		{
			WindowProperties properties;
			EventCallback eventCallback;
		};

	private:
		Data mData;
		tur_unique<GLFWwindow, WindowDeleter> mWindow;
	};
}

namespace tur
{
	using Window = BaseWindow<tur::platform::WindowGLFW>;
}