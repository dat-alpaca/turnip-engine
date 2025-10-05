#pragma once
#include "graphics/graphics_binder.hpp"
#include "platform/glfw/window_glfw.hpp"

#include <vector>
#include <vulkan/vulkan.hpp>

namespace tur::platform
{
	class GLFWBinderVulkan
	{
	public:
		void pre_initialize(const WindowingCapabilities& capabilities);
		void post_initialize(WindowGLFW* window);
		void present();

	public:
		VkSurfaceKHR get_vulkan_surface(vk::Instance instance);
		void destroy_vulkan_surface(vk::Instance instance, VkSurfaceKHR surface);

	public:
		std::vector<const char*> get_vulkan_extension_platform_surface_names();

	private:
		WindowGLFW* rWindow = nullptr;
	};
}

namespace tur
{
	using GraphicsBinder = BaseGraphicsBinder<platform::GLFWBinderVulkan, platform::WindowGLFW>;
}