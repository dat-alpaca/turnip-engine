#pragma once
#include <vulkan/vulkan.hpp>

#include "rhi/vulkan/requirements/physical_device.hpp"
#include "rhi/vulkan/vulkan_state.hpp"

namespace tur::vulkan
{
	void initialize_physical_device(VulkanState& state, const PhysicalDeviceRequirements& requirements);
}