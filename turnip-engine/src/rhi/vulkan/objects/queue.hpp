#pragma once
#include "graphics/objects/queue.hpp"
#include "graphics/types/queue_operations.hpp"
#include "rhi/vulkan/types/queue_family.hpp"
#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	struct Queue
	{
		vk::Queue queue;
		QueueOperation operations;
		queue_family_index familyIndex;
	};
}