#pragma once
#include <vulkan/vulkan.hpp>
#include "rhi/vulkan/utils/logger.hpp"

namespace tur::vulkan
{
	inline vk::Semaphore allocate_semaphore(vk::Device device)
	{
		TUR_ASS(device);

		vk::SemaphoreCreateInfo createInfo = {};
		return check_vk_object(device.createSemaphore(createInfo), "Semaphore");
	}
}