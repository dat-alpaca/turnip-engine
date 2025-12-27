#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "binder_vulkan_glfw.hpp"
#include "rhi/vulkan/vulkan_constants.hpp"

namespace tur::platform
{
	void GLFWBinderVulkan::pre_initialize(const WindowingCapabilities& capabilities)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}
	void GLFWBinderVulkan::post_initialize(WindowGLFW* window)
	{
		rWindow = window;
	}
	void GLFWBinderVulkan::present()
	{
		/* Blank */
	}

	VkSurfaceKHR GLFWBinderVulkan::get_vulkan_surface(vk::Instance instance)
	{
		VkSurfaceKHR surface;
		VkResult result = glfwCreateWindowSurface(instance, rWindow->get_window(), nullptr, &surface);

		if (result == VK_SUCCESS)
			return surface;

		TUR_LOG_CRITICAL("Failed to create window surface using GLFW and Vulkan");
		return nullptr;
	}
	void GLFWBinderVulkan::destroy_vulkan_surface(vk::Instance instance, VkSurfaceKHR surface)
	{
		vkDestroySurfaceKHR(instance, surface, nullptr);
	}

	std::vector<const char*> GLFWBinderVulkan::get_vulkan_extension_platform_surface_names()
	{
#ifdef TUR_PLATFORM_WIN32
		return {tur::vulkan::WIN32_SurfaceExtensionName};
#elif defined(TUR_PLATFORM_LINUX)
		return {tur::vulkan::XCB_SurfaceExtensionName, tur::vulkan::XLIB_SurfaceExtensionName,
				tur::vulkan::WAYLAND_SurfaceExtensionName};
#elif defined(TUR_PLATFORM_APPLE)
		return {METAL_SurfaceExtensionName};
#endif
	}
}