#include "window_glfw.hpp"
#include "core/event/events.hpp"
#include "events_glfw.hpp"

namespace tur::platform
{
	static void set_window_events(GLFWwindow* glfwWindow)
	{
		// Status:
		glfwSetWindowCloseCallback(
			glfwWindow,
			[](GLFWwindow* window)
			{
				auto* data = static_cast<WindowGLFW::Data*>(glfwGetWindowUserPointer(window));

				WindowCloseEvent event;
				data->eventCallback(event);
			}
		);

		glfwSetWindowMaximizeCallback(
			glfwWindow,
			[](GLFWwindow* window, int maximized)
			{
				auto* data = static_cast<WindowGLFW::Data*>(glfwGetWindowUserPointer(window));

				WindowMaximizedEvent event(maximized);
				data->eventCallback(event);
			}
		);

		glfwSetWindowFocusCallback(
			glfwWindow,
			[](GLFWwindow* window, int focus)
			{
				auto* data = static_cast<WindowGLFW::Data*>(glfwGetWindowUserPointer(window));

				WindowFocusEvent event(focus);
				data->eventCallback(event);
			}
		);

		// Size:
		glfwSetWindowSizeCallback(
			glfwWindow,
			[](GLFWwindow* window, int width, int height)
			{
				auto* data = static_cast<WindowGLFW::Data*>(glfwGetWindowUserPointer(window));

				data->properties.dimensions = {width, height};

				WindowResizeEvent event(width, height);
				data->eventCallback(event);
			}
		);

		glfwSetFramebufferSizeCallback(
			glfwWindow,
			[](GLFWwindow* window, int width, int height)
			{
				auto* data = static_cast<WindowGLFW::Data*>(glfwGetWindowUserPointer(window));

				data->properties.dimensions = {width, height};

				WindowFramebufferEvent event(width, height);
				data->eventCallback(event);
			}
		);

		glfwSetWindowContentScaleCallback(
			glfwWindow,
			[](GLFWwindow* window, float xScale, float yScale)
			{
				auto* data = static_cast<WindowGLFW::Data*>(glfwGetWindowUserPointer(window));

				WindowContentScaleEvent event(xScale, yScale);
				data->eventCallback(event);
			}
		);

		// Position:
		glfwSetWindowPosCallback(
			glfwWindow,
			[](GLFWwindow* window, int xPos, int yPos)
			{
				auto* data = static_cast<WindowGLFW::Data*>(glfwGetWindowUserPointer(window));

				data->properties.position = {xPos, yPos};

				WindowMovedEvent event(xPos, yPos);
				data->eventCallback(event);
			}
		);

		// Keyboard:
		glfwSetKeyCallback(
			glfwWindow,
			[](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				auto* data = static_cast<WindowGLFW::Data*>(glfwGetWindowUserPointer(window));

				switch (action)
				{
					case GLFW_PRESS:
					{
						Key convertedKey = translate_key(key);
						KeyboardMods convertedMods = translate_key_mods(mods);

						KeyPressedEvent event(convertedKey, convertedMods);
						data->eventCallback(event);
					}
					break;

					case GLFW_RELEASE:
					{
						Key convertedKey = translate_key(key);
						KeyboardMods convertedMods = translate_key_mods(mods);

						KeyReleasedEvent event(convertedKey, convertedMods);
						data->eventCallback(event);
					}
					break;

					case GLFW_REPEAT:
					{
						Key convertedKey = translate_key(key);
						KeyboardMods convertedMods = translate_key_mods(mods);

						KeyRepeatEvent event(convertedKey, convertedMods);
						data->eventCallback(event);
					}
					break;

					default:
					{
						TUR_LOG_WARN("Invalid keyboard action passed to window callback: {}", action);
					}
					break;
				}
			}
		);

		// Mouse:
		glfwSetCursorPosCallback(
			glfwWindow,
			[](GLFWwindow* window, double x, double y)
			{
				auto* data = static_cast<WindowGLFW::Data*>(glfwGetWindowUserPointer(window));

				MouseMovedEvent event(x, y);
				data->eventCallback(event);
			}
		);

		glfwSetMouseButtonCallback(
			glfwWindow,
			[](GLFWwindow* window, int button, int action, int mods)
			{
				auto* data = static_cast<WindowGLFW::Data*>(glfwGetWindowUserPointer(window));

				switch (action)
				{
					case GLFW_PRESS:
					{
						MouseButton convertedButton = translate_mouse_button(button);
						MousePressedEvent event(convertedButton);
						data->eventCallback(event);
					}
					break;

					case GLFW_RELEASE:
					{
						MouseButton convertedButton = translate_mouse_button(button);
						MouseReleasedEvent event(convertedButton);
						data->eventCallback(event);
					}
					break;

					default:
					{
						TUR_LOG_WARN("Invalid keyboard action passed to window callback: {}", action);
					}
					break;
				}
			}
		);

		glfwSetScrollCallback(
			glfwWindow,
			[](GLFWwindow* window, double xOffset, double yOffset)
			{
				auto* data = static_cast<WindowGLFW::Data*>(glfwGetWindowUserPointer(window));

				MouseScrollEvent event(xOffset, yOffset);
				data->eventCallback(event);
			}
		);
	}
}

namespace tur::platform
{
	void WindowGLFW::create(const WindowProperties& properties)
	{
		mWindow.reset(glfwCreateWindow(
			properties.dimensions.x, properties.dimensions.y, properties.title.c_str(), nullptr, nullptr
		));

		if (!mWindow)
		{
			TUR_LOG_CRITICAL("Failed to create a valid GLFW window. GLFW is probably uninitialized");
			return;
		}

		set_properties(properties);
		set_window_events(mWindow.get());

		if (properties.startsMaximized)
			maximize();
	}
	void WindowGLFW::destroy()
	{
		mWindow.reset();
	}

	void WindowGLFW::set_event_callback(EventCallback& callback)
	{
		mData.eventCallback = callback;
		glfwSetWindowUserPointer(mWindow.get(), &mData);
	}
	void WindowGLFW::set_properties(const WindowProperties& properties)
	{
		mData.properties = properties;

		// Current Size:
		glfwSetWindowSize(mWindow.get(), (int)properties.dimensions.x, (int)properties.dimensions.y);

		// Size Limits:
		glfwSetWindowSizeLimits(
			mWindow.get(), (int)properties.minSize.x, (int)properties.minSize.y, (int)properties.maxSize.x,
			(int)properties.maxSize.y
		);

		// Position:
		if (properties.position != invalid_size)
			glfwSetWindowPos(mWindow.get(), properties.position.x, properties.position.y);

		// Title:
		glfwSetWindowTitle(mWindow.get(), properties.title.c_str());
	}
	void WindowGLFW::poll_events()
	{
		glfwPollEvents();
	}
	void WindowGLFW::wait_events()
	{
		glfwWaitEvents();
	}

	void WindowGLFW::maximize()
	{
		glfwMaximizeWindow(mWindow.get());
	}

	void WindowGLFW::show()
	{
		glfwShowWindow(mWindow.get());
	}
	void WindowGLFW::hide()
	{
		glfwHideWindow(mWindow.get());
	}
	bool WindowGLFW::is_open() const
	{
		return !glfwWindowShouldClose(mWindow.get());
	}

	glm::vec2 WindowGLFW::get_dimensions()
	{
		int width, height;
		glfwGetWindowSize(mWindow.get(), &width, &height);
		return {width, height};
	}

	bool WindowGLFW::get_mouse_pressed(MouseButton mouseButton)
	{
		int state = glfwGetMouseButton(mWindow.get(), reverse_mouse_button(mouseButton));
		return state == GLFW_PRESS;
	}
	bool WindowGLFW::get_key_pressed(Key key)
	{
		return glfwGetKey(mWindow.get(), reverse_key(key)) == GLFW_PRESS;
	}
	glm::vec2 WindowGLFW::get_mouse_position()
	{
		double xPos, yPos;
		glfwGetCursorPos(mWindow.get(), &xPos, &yPos);
		return glm::vec2(xPos, yPos);
	}

	f64 WindowGLFW::get_time()
	{
		return glfwGetTime();
	}
}