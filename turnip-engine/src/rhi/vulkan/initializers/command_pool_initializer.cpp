#include "command_pool_initializer.hpp"

namespace tur::vulkan
{
	void initialize_command_pool(VulkanState& state)
	{
		vk::CommandPoolCreateInfo poolCreateInfo = {};
		{
			poolCreateInfo.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

			auto transferQueueOpt = state.queueList.get_queue(QueueOperation::TRANSFER);
			if (!transferQueueOpt.has_value())
				TUR_LOG_CRITICAL("Error when trying to retrieve a queue that supports TRANSFER operations");

			poolCreateInfo.queueFamilyIndex = transferQueueOpt.value().familyIndex;
		}

		try
		{
			state.commandPool = state.logicalDevice.createCommandPool(poolCreateInfo);
		}
		catch (vk::SystemError& err)
		{
			TUR_LOG_CRITICAL("Failed to create frame command pool. {}", err.what());
		}
	}

	vk::CommandPool initialize_command_pool(const QueueList& queueList, vk::Device device)
	{
		vk::CommandPoolCreateInfo poolCreateInfo = {};
		{
			poolCreateInfo.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

			auto transferQueueOpt = queueList.get_queue(QueueOperation::TRANSFER);
			if (!transferQueueOpt.has_value())
				TUR_LOG_CRITICAL("Error when trying to retrieve a queue that supports TRANSFER operations");

			poolCreateInfo.queueFamilyIndex = transferQueueOpt.value().familyIndex;
		}

		try
		{
			return device.createCommandPool(poolCreateInfo);
		}
		catch (vk::SystemError& err)
		{
			TUR_LOG_CRITICAL("Failed to create frame command pool. {}", err.what());
		}
	}
}