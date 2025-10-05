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
		capabilities.surfaceCapabilities = device.getSurfaceCapabilitiesKHR(surface);

		auto surfaceFormats = device.getSurfaceFormatsKHR(surface);

		capabilities.availableSurfaceFormats.reserve(surfaceFormats.size());
		for (const auto& surfaceFormat : surfaceFormats)
			capabilities.availableSurfaceFormats.push_back(surfaceFormat);

		auto presentModes = device.getSurfacePresentModesKHR(surface);
		capabilities.presentModes.reserve(presentModes.size());
		for (const auto& presentMode : presentModes)
			capabilities.presentModes.push_back(presentMode);

		return capabilities;
	}
}