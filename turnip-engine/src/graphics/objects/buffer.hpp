#pragma once
#include "common.hpp"

namespace tur
{
	enum class BufferIndexType : u8
	{
		UNSIGNED_BYTE,
		UNSIGNED_SHORT,
		UNSIGNED_INT
	};
}

namespace tur
{
	using buffer_handle = handle_type;

	enum class BufferType
	{
		NONE = 0,
		VERTEX_BUFFER = 1 << 0,
		INDEX_BUFFER = 1 << 1,
		UNIFORM_BUFFER = 1 << 2,
		STORAGE_BUFFER = 1 << 3,
		TRANSFER_SRC = 1 << 4,
		TRANSFER_DST = 1 << 5,
	};

	inline BufferType operator|(BufferType lhs, BufferType rhs)
	{
		return static_cast<BufferType>(static_cast<u32>(lhs) | static_cast<u32>(rhs));
	}
	inline BufferType operator&(BufferType lhs, BufferType rhs)
	{
		return static_cast<BufferType>(static_cast<u32>(lhs) & static_cast<u32>(rhs));
	}

	enum class BufferUsage
	{
		NONE = 0,
		DYNAMIC = 1 << 0,
		READ = 1 << 1,
		WRITE = 1 << 2,
		PERSISTENT = 1 << 3,
		COHERENT = 1 << 4,
	};

	inline BufferUsage operator|(BufferUsage lhs, BufferUsage rhs)
	{
		return static_cast<BufferUsage>(static_cast<u32>(lhs) | static_cast<u32>(rhs));
	}
	inline BufferUsage operator&(BufferUsage lhs, BufferUsage rhs)
	{
		return static_cast<BufferUsage>(static_cast<u32>(lhs) & static_cast<u32>(rhs));
	}

	enum class BufferMemoryUsage
	{
		CPU_ONLY = 0,
		GPU_ONLY,
		GPU_TO_CPU,
		CPU_TO_GPU
	};

	struct BufferDescriptor
	{
		BufferType type = BufferType::NONE;
		BufferUsage usage = BufferUsage::NONE;
		BufferMemoryUsage memoryUsage = BufferMemoryUsage::CPU_ONLY;
	};
}