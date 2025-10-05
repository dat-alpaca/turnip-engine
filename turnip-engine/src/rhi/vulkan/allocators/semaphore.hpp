#pragma once
#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	inline vk::Semaphore allocate_semaphore(vk::Device device)
	{
		vk::SemaphoreCreateInfo createInfo = {};
		return device.createSemaphore(createInfo);
	}
}