#pragma once
#include <vulkan/vulkan.hpp>

#include "config/vulkan_config.hpp"
#include "rhi/vulkan/vulkan_state.hpp"

namespace tur::vulkan
{
	void initialize_physical_device(VulkanState& state, const PhysicalDeviceRequirements& requirements);
}