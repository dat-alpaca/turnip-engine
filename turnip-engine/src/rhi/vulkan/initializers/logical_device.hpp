#pragma once
#include <vulkan/vulkan.hpp>

#include "core/config/vulkan_config.hpp"
#include "rhi/vulkan/vulkan_state.hpp"

namespace tur::vulkan
{
	void initialize_logical_device(VulkanState& state, const VulkanConfig& vulkanConfig);
}