#pragma once
#include <glad/gl.h>

#include "graphics/objects/descriptor.hpp"
#include "graphics/objects/pipeline.hpp"
#include "rhi/opengl/opengl_common.hpp"

namespace tur::gl
{
	constexpr inline gl_handle get_blend_func(BlendFunc blendFunction)
	{
		switch (blendFunction)
		{
			case BlendFunc::ADD:
				return GL_FUNC_ADD;
				
			case BlendFunc::SUBTRACT:
				return GL_FUNC_SUBTRACT;

			default:
				TUR_LOG_ERROR("Invalid Blend Function: {}. Default: GL_FUNC_ADD", static_cast<int>(blendFunction));
				return GL_FUNC_ADD;
		}
	}

	constexpr inline gl_handle get_blend_factor(BlendFactor blendFactor)
	{
		switch (blendFactor)
		{
			case BlendFactor::ZERO:
				return GL_ZERO;

			case BlendFactor::ONE:
				return GL_ONE;

			case BlendFactor::SRC_COLOR:
				return GL_SRC_COLOR;
				
			case BlendFactor::ONE_MINUS_SRC_COLOR:
				return GL_ONE_MINUS_SRC_COLOR;

			case BlendFactor::DST_COLOR:
				return GL_DST_COLOR;

			case BlendFactor::ONE_MINUS_DST_COLOR:
				return GL_ONE_MINUS_DST_COLOR;

			case BlendFactor::SRC_ALPHA:
				return GL_SRC_ALPHA;

			case BlendFactor::ONE_MINUS_SRC_ALPHA:
				return GL_ONE_MINUS_SRC_ALPHA;

			case BlendFactor::DST_ALPHA:
				return GL_DST_ALPHA;

			case BlendFactor::ONE_MINUS_DST_ALPHA:
				return GL_ONE_MINUS_DST_ALPHA;

			case BlendFactor::CONSTANT_COLOR:
				return GL_CONSTANT_COLOR;

			case BlendFactor::ONE_MINUS_CONSTANT_COLOR:
				return GL_ONE_MINUS_CONSTANT_COLOR;

			case BlendFactor::CONSTANT_ALPHA:
				return GL_CONSTANT_ALPHA;

			case BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
				return GL_ONE_MINUS_CONSTANT_ALPHA;

			case BlendFactor::SRC_ALPHA_SATURATE:
				return GL_SRC_ALPHA_SATURATE;

			case BlendFactor::SRC1_COLOR:
				return GL_SRC1_COLOR;

			case BlendFactor::ONE_MINUS_SRC1_COLOR:
				return GL_ONE_MINUS_SRC1_COLOR;

			case BlendFactor::SRC1_ALPHA:
				return GL_SRC1_ALPHA;

			case BlendFactor::ONE_MINUS_SRC1_ALPHA:
				return GL_ONE_MINUS_SRC1_ALPHA;

			default:
				TUR_LOG_ERROR("Invalid Blender Factor: {}. Default: GL_ZERO", static_cast<int>(blendFactor));
				return GL_ZERO;
		}
	}

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

	constexpr inline gl_handle get_compare_op(DepthCompareOp op)
	{
		switch(op)
		{
			case DepthCompareOp::NEVER:
				return GL_NEVER;

			case DepthCompareOp::ALWAYS:
				return GL_ALWAYS;

			case DepthCompareOp::EQUAL:
				return GL_EQUAL;

			case DepthCompareOp::NOT_EQUAL:
				return GL_NOTEQUAL;

			case DepthCompareOp::LESS:
				return GL_LESS;

			case DepthCompareOp::LESS_OR_EQUAL:
				return GL_LEQUAL;

			case DepthCompareOp::GREATER:
				return GL_GREATER;

			case DepthCompareOp::GREATER_OR_EQUAL:
				return GL_GEQUAL;
		}

		TUR_LOG_ERROR("Invalid Depth Compare Operation: {}. Default: GL_LESS", static_cast<int>(op));
		return GL_LESS;
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

			default:
				TUR_LOG_ERROR("Invalid Polygon Mode: {}. Default: GL_FILL", static_cast<int>(polygonMode));
				return GL_FILL;
		}
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

			case DescriptorType::STORAGE_BUFFER:
				return GL_SHADER_STORAGE_BUFFER;

			case DescriptorType::COMBINED_IMAGE_SAMPLER:
				return GL_UNIFORM_BUFFER;

			default:
				TUR_LOG_CRITICAL("Invalid Descriptor Type: {}", static_cast<u32>(type));
				return invalid_handle;
		}
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

			case AttributeFormat::R32_UINT:
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

			default:
				TUR_LOG_CRITICAL("Invalid Attribute Format [Size]: {}", static_cast<u64>(format));
				return invalid_handle;
		}
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
		DepthDescriptor depthDescriptor;
		BlendDescriptor blendDescriptor;

		descriptor_set_layout_handle setLayout;
		gl_handle handle = invalid_handle;
	};
}