#pragma once

#ifdef TUR_GRAPHICS_GL
	#include "opengl/gl_render_interface.hpp"
#elif defined(TUR_GRAPHICS_VK)
	#include "vulkan/vulkan_render_interface.hpp"
#endif