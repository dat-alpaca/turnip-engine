#pragma once
#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	inline vk::Semaphore allocate_semaphore(vk::Device device)
	{
		vk::SemaphoreCreateInfo createInfo = {};
		return check_vk_object(device.createSemaphore(createInfo), "Semaphore");
	}
}