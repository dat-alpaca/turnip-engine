#pragma once
#include <glad/gl.h>

#include "graphics/objects/shader.hpp"
#include "rhi/opengl/opengl_common.hpp"

namespace tur::gl
{
	struct Shader
	{
		gl_handle handle;
	};
}

namespace tur::gl
{
	constexpr inline gl_handle get_shader_type(ShaderType type)
	{
		switch (type)
		{
			case ShaderType::VERTEX:
				return GL_VERTEX_SHADER;

			case ShaderType::TESSELATION_CONTROL:
				return GL_TESS_CONTROL_SHADER;

			case ShaderType::TESSELATION_EVALUATION:
				return GL_TESS_EVALUATION_SHADER;

			case ShaderType::GEOMETRY:
				return GL_GEOMETRY_SHADER;

			case ShaderType::FRAGMENT:
				return GL_FRAGMENT_SHADER;

			case ShaderType::COMPUTE:
				return GL_COMPUTE_SHADER;

			default:
				TUR_LOG_CRITICAL("OpenGL does not support the following shader type: {}", (u32)type);
		}

		return invalid_handle;
	}
}