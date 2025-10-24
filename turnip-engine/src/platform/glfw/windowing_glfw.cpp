#include "windowing_glfw.hpp"

static inline void GLFWErrorCallback(int errorCode, const char* description)
{
	TUR_LOG_ERROR("[GLFW]: {} [{}]", description, errorCode);
}

namespace tur::platform
{
	tur_unique<Window> WindowingGLFW::initialize(const WindowingCapabilities& capabilities,
												 const WindowProperties& properties)
	{
		glfwSetErrorCallback(GLFWErrorCallback);

		if (!glfwInit())
			TUR_LOG_CRITICAL("Failed to initialize the platform system.");

#ifdef TUR_DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

#ifdef TUR_PLATFORM_MACOSX
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		sBinder.pre_initialize(capabilities);

		glfwWindowHint(GLFW_SCALE_TO_MONITOR, capabilities.scaleToMonitor);
		glfwWindowHint(GLFW_SRGB_CAPABLE, capabilities.srgbCapable);
		glfwWindowHint(GLFW_RESIZABLE, capabilities.resizable);
		glfwWindowHint(GLFW_VISIBLE, capabilities.startsVisible);

		tur_unique<Window> window = tur::make_unique<Window>();
		{
			window->create(properties);
			window->show();
		}

		sBinder.post_initialize(window.get());

		return window;
	}
	void WindowingGLFW::shutdown()
	{
		glfwTerminate();
	}
}