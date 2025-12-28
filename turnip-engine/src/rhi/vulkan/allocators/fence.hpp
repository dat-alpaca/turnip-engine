#pragma once
#include "rhi/vulkan/utils/logger.hpp"
#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	inline vk::Fence allocate_signaled_fence(vk::Device device)
	{
		TUR_ASS(device);
		
		vk::FenceCreateInfo createInfo = {};
		createInfo.flags = vk::FenceCreateFlagBits::eSignaled;

		return check_vk_object(device.createFence(createInfo), "Fence");
	}
}