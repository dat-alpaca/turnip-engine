#pragma once
#include "graphics/objects/buffer.hpp"
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	struct Buffer
	{
		vk::Buffer buffer;
		VmaAllocation allocation;
		VmaAllocationInfo allocationInfo;

		u32 size = 0;
	};
}

namespace tur::vulkan
{
	inline vk::IndexType get_buffer_index_type(BufferIndexType indexType)
	{
		switch (indexType)
		{
			case BufferIndexType::UNSIGNED_BYTE:
				return vk::IndexType::eUint16;

			case BufferIndexType::UNSIGNED_SHORT:
				return vk::IndexType::eUint16;

			case BufferIndexType::UNSIGNED_INT:
				return vk::IndexType::eUint32;
		}

		TUR_LOG_ERROR("Invalid buffer index type: {}. Default: eUint32", static_cast<u32>(indexType));
		return vk::IndexType::eUint32;
	}
}

namespace tur::vulkan
{
	constexpr static inline vk::BufferUsageFlags get_buffer_usage(BufferType type)
	{
		u32 valueFlags = 0;
		u32 typeValue = static_cast<u32>(type);

		if (typeValue & static_cast<u32>(BufferType::VERTEX_BUFFER))
			valueFlags |= static_cast<u32>(vk::BufferUsageFlagBits::eVertexBuffer);

		if (typeValue & static_cast<u32>(BufferType::INDEX_BUFFER))
			valueFlags |= static_cast<u32>(vk::BufferUsageFlagBits::eIndexBuffer);

		if (typeValue & static_cast<u32>(BufferType::UNIFORM_BUFFER))
			valueFlags |= static_cast<u32>(vk::BufferUsageFlagBits::eUniformBuffer);

		if (typeValue & static_cast<u32>(BufferType::TRANSFER_SRC))
			valueFlags |= static_cast<u32>(vk::BufferUsageFlagBits::eTransferSrc);

		if (typeValue & static_cast<u32>(BufferType::TRANSFER_DST))
			valueFlags |= static_cast<u32>(vk::BufferUsageFlagBits::eTransferDst);

		return vk::BufferUsageFlags(valueFlags);
	}

	constexpr static inline VmaAllocationCreateFlags get_buffer_memory_flags(BufferUsage usage)
	{
		VmaAllocationCreateFlags flags = 0;
		if (static_cast<u32>(usage) & static_cast<u32>(BufferUsage::PERSISTENT))
			flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;

		return flags;
	}

	constexpr static inline VmaMemoryUsage get_buffer_memory_usage(BufferMemoryUsage memoryUsage)
	{
		switch (memoryUsage)
		{
			case BufferMemoryUsage::CPU_ONLY:
				return VMA_MEMORY_USAGE_CPU_ONLY;

			case BufferMemoryUsage::GPU_ONLY:
				return VMA_MEMORY_USAGE_GPU_ONLY;

			case BufferMemoryUsage::GPU_TO_CPU:
				return VMA_MEMORY_USAGE_GPU_TO_CPU;

			case BufferMemoryUsage::CPU_TO_GPU:
				return VMA_MEMORY_USAGE_CPU_TO_GPU;
		}

		TUR_LOG_ERROR("Invalid buffer memory usage: {}. Default: VMA_CPU_ONLY", static_cast<u32>(memoryUsage));
		return VMA_MEMORY_USAGE_CPU_ONLY;
	}
}