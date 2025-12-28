#pragma once
#include <vulkan/vulkan.hpp>

#include "core/config/config_data.hpp"
#include "rhi/vulkan/vulkan_state.hpp"

namespace tur::vulkan
{
	void initialize_instance(VulkanState& state, const ConfigData& configuration);

	void destroy_instance_messenger(VulkanState& state);
}