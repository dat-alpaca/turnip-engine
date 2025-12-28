#pragma once
#include "rhi/vulkan/utils/logger.hpp"
#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	inline vk::CommandBuffer allocate_single_primary_command_buffer(vk::Device device, vk::CommandPool commandPool)
	{
		TUR_ASS(device);
		TUR_ASS(commandPool);

		vk::CommandBufferAllocateInfo allocateInfo = {};
		{
			allocateInfo.commandPool = commandPool;
			allocateInfo.commandBufferCount = 1;
			allocateInfo.level = vk::CommandBufferLevel::ePrimary;
		}

		return check_vk_object(device.allocateCommandBuffers(allocateInfo), "CommandBuffer").front();
	}

	inline vk::CommandBuffer allocate_single_secondary_command_buffer(vk::Device device, vk::CommandPool commandPool)
	{
		TUR_ASS(device);
		TUR_ASS(commandPool);

		vk::CommandBufferAllocateInfo allocateInfo = {};
		{
			allocateInfo.commandPool = commandPool;
			allocateInfo.commandBufferCount = 1;
			allocateInfo.level = vk::CommandBufferLevel::eSecondary;
		}

		return check_vk_object(device.allocateCommandBuffers(allocateInfo), "CommandBuffer").front();
	}
}