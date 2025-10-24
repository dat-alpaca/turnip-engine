#pragma once
#include <glad/gl.h>

#include "graphics/objects/buffer.hpp"
#include "graphics/types/access_flags.hpp"
#include "rhi/opengl/opengl_common.hpp"

namespace tur::gl
{
	struct Buffer
	{
		BufferDescriptor descriptor = {};
		gl_handle handle = invalid_handle;
	};
}

namespace tur::gl
{
	constexpr static inline gl_handle get_buffer_index_type(BufferIndexType type)
	{
		switch (type)
		{
			case BufferIndexType::UNSIGNED_BYTE:
				return GL_UNSIGNED_BYTE;

			case BufferIndexType::UNSIGNED_SHORT:
				return GL_UNSIGNED_SHORT;

			case BufferIndexType::UNSIGNED_INT:
				return GL_UNSIGNED_INT;
		}

		TUR_LOG_ERROR("Invalid Index Buffer Type: {}. Default: GL_UNSIGNED_INT", static_cast<int>(type));

		return GL_UNSIGNED_INT;
	}
}

namespace tur::gl
{
	static inline gl_handle get_buffer_usage(BufferUsage usage)
	{
		u32 flags = 0;

		if (static_cast<u32>(usage) & static_cast<u32>(BufferUsage::DYNAMIC))
			flags |= GL_DYNAMIC_STORAGE_BIT;

		if (static_cast<u32>(usage) & static_cast<u32>(BufferUsage::READ))
			flags |= GL_MAP_READ_BIT;

		if (static_cast<u32>(usage) & static_cast<u32>(BufferUsage::WRITE))
			flags |= GL_MAP_WRITE_BIT;

		// todo: specify later
		if (static_cast<u32>(usage) & static_cast<u32>(BufferUsage::PERSISTENT))
		{
			flags |= GL_MAP_PERSISTENT_BIT;
			flags |= GL_MAP_READ_BIT;
			flags |= GL_MAP_WRITE_BIT;
			flags |= GL_DYNAMIC_STORAGE_BIT;
		}

		if (static_cast<u32>(usage) & static_cast<u32>(BufferUsage::COHERENT))
		{
			flags |= GL_MAP_COHERENT_BIT;
			flags |= GL_MAP_PERSISTENT_BIT;
			flags |= GL_MAP_READ_BIT;
			flags |= GL_MAP_WRITE_BIT;
			flags |= GL_DYNAMIC_STORAGE_BIT;
		}

		return flags;
	}
}

namespace tur::gl
{
	static inline gl_handle get_buffer_access_flags(AccessFlags flags)
	{
		u32 result = 0;

		if (static_cast<u32>(flags) & static_cast<u32>(AccessFlags::READ))
			result |= GL_MAP_READ_BIT;

		if (static_cast<u32>(flags) & static_cast<u32>(AccessFlags::WRITE))
			result |= GL_MAP_WRITE_BIT;

		if (static_cast<u32>(flags) & static_cast<u32>(AccessFlags::PERSISTENT))
			result |= GL_MAP_PERSISTENT_BIT;

		if (static_cast<u32>(flags) & static_cast<u32>(AccessFlags::COHERENT))
			result |= GL_MAP_COHERENT_BIT;

		return result;
	}
}