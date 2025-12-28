#pragma once
#include <vulkan/vulkan.hpp>

#include "graphics/types/extent.hpp"
#include "rhi/vulkan/vulkan_state.hpp"

namespace tur::vulkan
{
	struct SwapchainRequirements
	{
		Extent2D extent;
		u32 imageCount = invalid_handle;

		vk::PresentModeKHR presentMode = vk::PresentModeKHR::eMailbox;
		VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE;
	};

	void initialize_swapchain(VulkanState& state, const SwapchainRequirements& requirements);

	void destroy_swapchain(VulkanState& state);
}