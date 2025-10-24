#pragma once
#include "graphics/graphics_binder.hpp"
#include "platform/glfw/window_glfw.hpp"

namespace tur::platform
{
	class GLFWBinderGL
	{
	public:
		void pre_initialize(const WindowingCapabilities& capabilities);
		void post_initialize(WindowGLFW* window);
		void present();

	private:
		WindowingCapabilities mCapabilities;
		WindowGLFW* rWindow = nullptr;
	};
}

namespace tur
{
	using GraphicsBinder = BaseGraphicsBinder<platform::GLFWBinderGL, platform::WindowGLFW>;
}