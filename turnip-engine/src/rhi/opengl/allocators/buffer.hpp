#pragma once

#include "common.hpp"
#include "rhi/opengl/objects/buffer.hpp"
#include "rhi/opengl/opengl_common.hpp"
#include <glad/gl.h>

namespace tur::gl
{
	inline Buffer allocate_buffer(const BufferDescriptor& descriptor, const std::vector<byte>& data)
	{
		gl_handle bufferID;
		glCreateBuffers(1, &bufferID);
		glNamedBufferStorage(bufferID, data.size(), data.data(), get_buffer_usage(descriptor.usage));

		Buffer buffer;
		buffer.descriptor = descriptor;
		buffer.handle = bufferID;

		return buffer;
	}
}