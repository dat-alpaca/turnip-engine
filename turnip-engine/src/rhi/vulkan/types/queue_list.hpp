#pragma once
#include "graphics/types/queue_operations.hpp"
#include "rhi/vulkan/objects/queue.hpp"

#include <optional>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	class QueueList
	{
	public:
		Queue get_queue(queue_handle handle)
		{
			TUR_ASSERT(handle >= 0 && handle < mQueues.size(), "Invalid queue index");
			return mQueues[handle];
		}
		std::optional<Queue> get_queue(QueueOperation operation) const
		{
			for (const auto& queue : mQueues)
			{
				if ((queue.operations & operation))
					return queue;
			}

			return std::nullopt;
		}
		queue_handle get_queue_index(QueueOperation operation)
		{
			for (u64 i = 0; i < mQueues.size(); ++i)
			{
				if ((mQueues[i].operations & operation))
					return static_cast<queue_handle>(i);
			}

			return invalid_handle;
		}

		void register_queue(const Queue& queue) { mQueues.push_back(queue); }

	public:
		std::vector<Queue> mQueues;
	};
}