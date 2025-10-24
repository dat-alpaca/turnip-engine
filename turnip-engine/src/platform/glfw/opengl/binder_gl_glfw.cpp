#include "binder_gl_glfw.hpp"
#include <GLFW/glfw3.h>
#include <glad/gl.h>

static void OpenGLCallback(unsigned, unsigned type, unsigned, unsigned severity, int, const char* message, const void*)
{
	fprintf(
		stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x\n", (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity
	);

	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:
			TUR_LOG_CRITICAL(message);
			break;

		case GL_DEBUG_SEVERITY_MEDIUM:
			TUR_LOG_ERROR(message);
			break;

		case GL_DEBUG_SEVERITY_LOW:
			TUR_LOG_WARN(message);
			break;

		case GL_DEBUG_SEVERITY_NOTIFICATION:
			TUR_LOG_INFO(message);
			break;
	}

	TUR_LOG_INFO("[OpenGL][Unknown severity]: {}", message);
}

namespace tur::platform
{
	void GLFWBinderGL::pre_initialize(const WindowingCapabilities& capabilities)
	{
		mCapabilities = capabilities;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, capabilities.major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, capabilities.minor);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		TUR_LOG_DEBUG("PreInitialized GLFW GL Binder");
	}
	void GLFWBinderGL::post_initialize(WindowGLFW* window)
	{
		rWindow = window;
		glfwMakeContextCurrent(window->get_window());

		if (!gladLoadGL(glfwGetProcAddress))
			TUR_LOG_CRITICAL("Failed to load OpenGL functions.");

#ifdef TUR_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		if (mCapabilities.major >= 4 && mCapabilities.minor >= 3)
		{
			glDebugMessageCallback(OpenGLCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
		}
#endif

		TUR_LOG_DEBUG("PostInitialized GLFW GL Binder");
	}
	void GLFWBinderGL::present()
	{
		glfwSwapBuffers(rWindow->get_window());
	}
}