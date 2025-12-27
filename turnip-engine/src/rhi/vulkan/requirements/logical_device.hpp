#pragma once
#include "graphics/types/queue_operations.hpp"
#include <vector>

namespace tur::vulkan
{
	struct LogicalDeviceRequirements
	{
		bool customQueueRequirements = false;
		std::vector<QueueOperation> requiredQueueOperations = {QueueOperation::GRAPHICS, QueueOperation::PRESENT};
		std::vector<const char*> extensions;
	};
}