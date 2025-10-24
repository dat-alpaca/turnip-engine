#pragma once
#include "graphics/types/queue_operations.hpp"

namespace tur::vulkan
{
	struct LogicalDeviceRequirements
	{
		bool customQueueRequirements = false;
		std::vector<QueueOperation> requiredQueueOperations = {QueueOperation::GRAPHICS, QueueOperation::PRESENT};
	};
}