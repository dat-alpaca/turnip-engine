#pragma once
#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	inline vk::Fence allocate_signaled_fence(vk::Device device)
	{
		vk::FenceCreateInfo createInfo = {};
		createInfo.flags = vk::FenceCreateFlagBits::eSignaled;
		return device.createFence(createInfo);
	}
}