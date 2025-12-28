#pragma once
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include "graphics/objects/buffer.hpp"
#include "rhi/vulkan/objects/buffer.hpp"

namespace tur::vulkan
{
	inline Buffer allocate_buffer(VmaAllocator allocator, const BufferDescriptor& descriptor, u32 size)
	{
		TUR_ASS(allocator);
		TUR_ASS(size > 0);

		TUR_ASS(descriptor.type != BufferType::NONE);

		vk::BufferCreateInfo createInfo = {};
		{
			createInfo.size = size;
			createInfo.usage = get_buffer_usage(descriptor.type);
		}

		VmaAllocationCreateInfo vmaAllocationCreateInfo = {};
		{
			vmaAllocationCreateInfo.usage = get_buffer_memory_usage(descriptor.memoryUsage);
			vmaAllocationCreateInfo.flags = get_buffer_memory_flags(descriptor.usage);
		}

		Buffer buffer = {};
		buffer.size = size;

		VkResult result = vmaCreateBuffer(
			allocator, reinterpret_cast<VkBufferCreateInfo*>(&createInfo), &vmaAllocationCreateInfo,
			reinterpret_cast<VkBuffer*>(&buffer.buffer), &buffer.allocation, &buffer.allocationInfo
		);

		if (result != VK_SUCCESS)
			TUR_LOG_CRITICAL("Failed to create buffer.");

		return buffer;
	}
}