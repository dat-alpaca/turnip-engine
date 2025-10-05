#pragma once
#include <limits>
#include <numeric>
#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	static inline constexpr const char* ValidationLayerName = "VK_LAYER_KHRONOS_validation";
	static inline constexpr const char* DebugUtilsExtensionName = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
	static inline constexpr const char* SwapchainExtensionName = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

	static inline constexpr const char* SurfaceExtensionName = "VK_KHR_surface";
	static inline constexpr const char* WIN32_SurfaceExtensionName = "VK_KHR_win32_surface";
	static inline constexpr const char* ANDROID_SurfaceExtensionName = "VK_KHR_android_surface";

	static inline constexpr const char* XCB_SurfaceExtensionName = "VK_KHR_xcb_surface";
	static inline constexpr const char* XLIB_SurfaceExtensionName = "VK_KHR_xlib_surface";
	static inline constexpr const char* WAYLAND_SurfaceExtensionName = "VK_KHR_wayland_surface";

	static inline constexpr const char* METAL_SurfaceExtensionName = "VK_EXT_metal_surface";

	static inline constexpr auto DefaultDebugMessageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
															   | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
															   | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
	static inline constexpr auto DefaultDebugMessageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
														   | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
														   | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
}
