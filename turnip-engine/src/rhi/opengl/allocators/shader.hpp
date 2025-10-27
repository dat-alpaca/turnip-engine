#pragma once
#include "common.hpp"
#include "rhi/opengl/objects/shader.hpp"
#include "utils/file.hpp"

namespace
{
	inline void check_compile_error(tur::gl::gl_handle shader, tur::ShaderType type)
	{
		using namespace tur;
		constexpr u64 BufferSize = 256;

		int success;
		char infoLog[BufferSize];

		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, BufferSize, NULL, infoLog);
			TUR_LOG_ERROR("Failed to compile {} Shader: {}", gl::get_shader_type(type), infoLog);
		}
	}
}

namespace tur::gl
{
	inline Shader allocate_shader(ShaderType shaderType, const std::string& shaderContents)
	{
		const char* cString = shaderContents.c_str();

		u32 shaderID = glCreateShader(get_shader_type(shaderType));
		glShaderSource(shaderID, 1, &cString, NULL);
		glCompileShader(shaderID);
		check_compile_error(shaderID, shaderType);

		return Shader{.handle = shaderID};
	}
}