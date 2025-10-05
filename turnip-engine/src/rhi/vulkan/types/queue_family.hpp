#pragma once
#include "graphics/types/queue_operations.hpp"
#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	using queue_family_index = handle_type;

	struct QueueFamily
	{
		queue_family_index index;
		QueueOperation supportedOperations;
	};

	inline QueueOperation get_queue_family_supported_features(const vk::PhysicalDevice& device, VkSurfaceKHR surface)
	{
		QueueOperation support;

		queue_family_index queueIndex = 0;
		for (const auto& queueFamily : device.getQueueFamilyProperties())
		{
			if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
				support |= QueueOperation::GRAPHICS;

			if (device.getSurfaceSupportKHR(queueIndex, surface))
				support |= QueueOperation::PRESENT;

			if (queueFamily.queueFlags & vk::QueueFlagBits::eCompute)
				support |= QueueOperation::COMPUTE;

			if (queueFamily.queueFlags & vk::QueueFlagBits::eTransfer)
				support |= QueueOperation::TRANSFER;

			if (queueFamily.queueFlags & vk::QueueFlagBits::eSparseBinding)
				support |= QueueOperation::SPARSE_BINDING;

			++queueIndex;
		}

		return static_cast<QueueOperation>(support);
	}

	inline std::vector<QueueFamily> get_queue_families(const vk::PhysicalDevice& device, VkSurfaceKHR surface)
	{
		std::vector<QueueFamily> queueFamilies;

		queue_family_index queueIndex = 0;
		for (const auto& queueFamily : device.getQueueFamilyProperties())
		{
			QueueOperation support;

			if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
			{
				support |= QueueOperation::GRAPHICS;
				support |= QueueOperation::TRANSFER;
			}

			if (device.getSurfaceSupportKHR(queueIndex, surface))
				support |= QueueOperation::PRESENT;

			if (queueFamily.queueFlags & vk::QueueFlagBits::eCompute)
			{
				support |= QueueOperation::COMPUTE;
				support |= QueueOperation::TRANSFER;
			}

			if (queueFamily.queueFlags & vk::QueueFlagBits::eTransfer)
				support |= QueueOperation::TRANSFER;

			if (queueFamily.queueFlags & vk::QueueFlagBits::eSparseBinding)
				support |= QueueOperation::SPARSE_BINDING;

			queueFamilies.push_back({queueIndex, static_cast<QueueOperation>(support)});
			++queueIndex;
		}

		return queueFamilies;
	}
}