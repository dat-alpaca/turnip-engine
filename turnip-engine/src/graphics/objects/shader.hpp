#pragma once
#include "common.hpp"
#include <filesystem>

namespace tur
{
	using shader_handle = handle_type;

	enum class ShaderType
	{
		UNKNOWN = 0,
		VERTEX,
		TESSELATION_CONTROL,
		TESSELATION_EVALUATION,
		GEOMETRY,
		FRAGMENT,
		COMPUTE,
	};

	struct ShaderDescriptor
	{
		std::filesystem::path filepath;
		ShaderType type;
	};

	constexpr inline const char* get_shader_type_name(ShaderType type)
	{
		switch (type)
		{
			case ShaderType::VERTEX:
				return "VERTEX";

			case ShaderType::TESSELATION_CONTROL:
				return "TESSELATION_CONTROL";

			case ShaderType::TESSELATION_EVALUATION:
				return "TESSELATION_EVALUATION";

			case ShaderType::GEOMETRY:
				return "GEOMETRY";

			case ShaderType::FRAGMENT:
				return "FRAGMENT";

			case ShaderType::COMPUTE:
				return "COMPUTE";

			default:
				return "UNKNOWN";
		}

		return "UNKNOWN";
	}
}