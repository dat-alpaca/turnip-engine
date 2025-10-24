#pragma once
#include <glad/gl.h>

#include "graphics/objects/descriptor.hpp"
#include "graphics/objects/pipeline.hpp"
#include "rhi/opengl/opengl_common.hpp"

namespace tur::gl
{
	constexpr inline gl_handle get_front_face(FrontFace frontFace)
	{
		switch (frontFace)
		{
			case FrontFace::COUNTER_CLOCKWISE:
				return GL_CCW;
			case FrontFace::CLOCKWISE:
				return GL_CW;
		}

		TUR_LOG_ERROR("Invalid Front Face: {}. Default: GL_CCW", static_cast<int>(frontFace));

		return GL_CCW;
	}

	constexpr inline gl_handle get_cull_mode(CullMode cullMode)
	{
		switch (cullMode)
		{
			case CullMode::NONE:
				return GL_NONE;

			case CullMode::FRONT:
				return GL_FRONT;

			case CullMode::BACK:
				return GL_BACK;

			case CullMode::FRONT_AND_BACK:
				return GL_FRONT_AND_BACK;
		}

		TUR_LOG_ERROR("Invalid Cull Mode: {}. Default: GL_FRONT", static_cast<int>(cullMode));

		return GL_FRONT;
	}

	constexpr inline gl_handle get_polygon_mode(PolygonMode polygonMode)
	{
		switch (polygonMode)
		{
			case PolygonMode::FILL:
				return GL_FILL;

			case PolygonMode::LINE:
				return GL_LINE;

			case PolygonMode::POINT:
				return GL_POINT;
		}

		TUR_LOG_ERROR("Invalid Polygon Mode: {}. Default: GL_FILL", static_cast<int>(polygonMode));

		return GL_FILL;
	}

	constexpr inline gl_handle get_primitive_topology(PrimitiveTopology topology)
	{
		// TODO: warnings with specific primitive topology.

		switch (topology)
		{
			case PrimitiveTopology::POINTS:
				return GL_POINTS;

			case PrimitiveTopology::LINES:
				return GL_LINES;

			case PrimitiveTopology::LINE_STRIPS:
				return GL_LINE_STRIP;

			case PrimitiveTopology::TRIANGLES:
				return GL_TRIANGLES;

			case PrimitiveTopology::TRIANGLE_STRIPS:
				return GL_TRIANGLE_STRIP;

			case PrimitiveTopology::TRIANGLE_FANS:
				return GL_TRIANGLE_FAN;

			case PrimitiveTopology::LINES_WITH_ADJACENCY:
				return GL_LINES;

			case PrimitiveTopology::LINE_STRIPS_WITH_ADJACENCY:
				return GL_LINES;

			case PrimitiveTopology::TRIANGLES_WITH_ADJACENCY:
				return GL_TRIANGLES;

			case PrimitiveTopology::TRIANGLE_STRIPS_WITH_ADJACENCY:
				return GL_TRIANGLE_STRIP;

			case PrimitiveTopology::PATCHES:
				return GL_PATCHES;
		}

		TUR_LOG_ERROR("Invalid Topology: {}. Default: GL_TRIANGLES", static_cast<int>(topology));

		return GL_TRIANGLES;
	}
}

namespace tur::gl
{
	constexpr inline gl_handle get_descriptor_set_type(DescriptorType type)
	{
		switch (type)
		{
			case DescriptorType::UNIFORM_BUFFER:
				return GL_UNIFORM_BUFFER;

			case DescriptorType::COMBINED_IMAGE_SAMPLER:
				return GL_UNIFORM_BUFFER;
		}

		TUR_LOG_CRITICAL("Invalid Descriptor Type: {}", static_cast<u32>(type));
		return invalid_handle;
	}

	constexpr inline gl_handle get_attribute_format(AttributeFormat format)
	{
		switch (format)
		{
			case AttributeFormat::R32_SFLOAT:
				return GL_FLOAT;

			case AttributeFormat::R64_SFLOAT:
				return GL_FLOAT;

			case AttributeFormat::R32G32_SFLOAT:
				return GL_FLOAT;

			case AttributeFormat::R32G32B32_SFLOAT:
				return GL_FLOAT;

			case AttributeFormat::R32G32B32A32_SFLOAT:
				return GL_FLOAT;

			case AttributeFormat::R32G32_SINT:
				return GL_INT;

			case AttributeFormat::R32G32B32A32_UINT:
				return GL_UNSIGNED_INT;
		}

		TUR_LOG_CRITICAL("Invalid Attribute Format: {}", static_cast<u32>(format));
		return invalid_handle;
	}

	constexpr inline u64 get_attribute_format_size(AttributeFormat format)
	{
		switch (format)
		{
			case AttributeFormat::R32_SFLOAT:
				return 1;

			case AttributeFormat::R64_SFLOAT:
				return 1;

			case AttributeFormat::R32G32_SFLOAT:
				return 2;

			case AttributeFormat::R32G32B32_SFLOAT:
				return 3;

			case AttributeFormat::R32G32B32A32_SFLOAT:
				return 4;

			case AttributeFormat::R32G32_SINT:
				return 2;

			case AttributeFormat::R32G32B32A32_UINT:
				return 4;
		}

		TUR_LOG_CRITICAL("Invalid Attribute Format [Size]: {}", static_cast<u64>(format));
		return invalid_handle;
	}
}

namespace tur::gl
{
	inline void check_program_link_errors(u32 program)
	{
		constexpr u64 BufferSize = 256;

		int success;
		char infoLog[BufferSize];

		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(program, BufferSize, NULL, infoLog);
			TUR_LOG_ERROR("Failed to link shader program: {}", infoLog);
		}
	}
}

namespace tur::gl
{
	struct Pipeline
	{
		VertexInputDescriptor vertexInputStage;
		InputAssemblyDescriptor inputAssemblyStage;
		RasterizerDescriptor rasterizerStage;

		descriptor_set_layout_handle setLayout;
		gl_handle handle = invalid_handle;
	};
}