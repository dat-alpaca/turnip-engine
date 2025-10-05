#pragma once
#include "common.hpp"
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

		try
		{
			return device.allocateCommandBuffers(allocateInfo).front();
		}
		catch (vk::SystemError err)
		{
			TUR_LOG_CRITICAL("Failed to allocate frame command buffer: {}", err.what());
		}
	}
}