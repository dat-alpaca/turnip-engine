#pragma once
#include <vulkan/vulkan.hpp>
#include "rhi/vulkan/vulkan_state.hpp"

namespace tur::vulkan
{
	void initialize_command_pool(VulkanState& state);
	vk::CommandPool initialize_command_pool(const QueueList& queueList, vk::Device device);
}