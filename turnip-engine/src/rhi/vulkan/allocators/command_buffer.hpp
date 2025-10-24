#pragma once
#include "common.hpp"
#include "rhi/vulkan/utils/logger.hpp"
#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	inline vk::CommandBuffer allocate_single_primary_command_buffer(vk::Device device, vk::CommandPool commandPool)
	{
		vk::CommandBufferAllocateInfo allocateInfo = {};
		{
			allocateInfo.commandPool = commandPool;
			allocateInfo.commandBufferCount = 1;
			allocateInfo.level = vk::CommandBufferLevel::ePrimary;
		}

		return check_vk_object(device.allocateCommandBuffers(allocateInfo), "CommandBuffer").front();
	}
}