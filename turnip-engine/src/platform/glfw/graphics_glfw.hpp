#pragma once

#ifdef TUR_GRAPHICS_GL
	#include "opengl/binder_gl_glfw.hpp"

#elif TUR_GRAPHICS_VK
	#include "vulkan/binder_vulkan_glfw.hpp"

#endif