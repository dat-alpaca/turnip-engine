#include "command_pool_initializer.hpp"
#include "rhi/vulkan/utils/logger.hpp"

namespace tur::vulkan
{
	void initialize_command_pool(VulkanState& state)
	{
		vk::CommandPoolCreateInfo poolCreateInfo = {};
		{
			poolCreateInfo.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

			auto transferQueueOpt = state.queueList.get_queue(QueueOperation::GRAPHICS);
			if (!transferQueueOpt.has_value())
				TUR_LOG_CRITICAL("Error when trying to retrieve a queue that supports TRANSFER operations");

			poolCreateInfo.queueFamilyIndex = transferQueueOpt.value().familyIndex;
		}

		state.commandPool = check_vk_object(state.logicalDevice.createCommandPool(poolCreateInfo), "CommandPool");
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

		return check_vk_object(device.createCommandPool(poolCreateInfo), "CommandPool");
	}
}