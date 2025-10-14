#pragma once
#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	struct SwapchainFeatures
	{
		vk::SurfaceCapabilitiesKHR surfaceCapabilities;
		std::vector<vk::SurfaceFormatKHR> availableSurfaceFormats;
		std::vector<vk::PresentModeKHR> presentModes;
	};

	inline SwapchainFeatures fetch_swapchain_features(vk::PhysicalDevice device, VkSurfaceKHR surface)
	{
		SwapchainFeatures capabilities;
		capabilities.surfaceCapabilities =
			check_vk_object(device.getSurfaceCapabilitiesKHR(surface), "SurfaceFeatures");

		auto surfaceFormats = check_vk_object(device.getSurfaceFormatsKHR(surface), "SurfaceFormats");
		capabilities.availableSurfaceFormats.reserve(surfaceFormats.size());
		for (const auto& surfaceFormat : surfaceFormats)
			capabilities.availableSurfaceFormats.push_back(surfaceFormat);

		auto presentModes = check_vk_object(device.getSurfacePresentModesKHR(surface), "SurfacePresentModes");
		capabilities.presentModes.reserve(presentModes.size());
		for (const auto& presentMode : presentModes)
			capabilities.presentModes.push_back(presentMode);

		return capabilities;
	}
}