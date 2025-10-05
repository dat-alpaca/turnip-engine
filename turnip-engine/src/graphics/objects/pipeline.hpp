#pragma once
#include <span>

#include "common.hpp"
#include "graphics/objects/descriptor.hpp"
#include "graphics/types/pipeline_stage.hpp"
#include "graphics/types/pipeline_type.hpp"

#include "graphics/types/extent.hpp"
#include "graphics/types/viewport.hpp"

#include "graphics/objects/shader.hpp"

namespace tur
{
	enum class InputRate
	{
		VERTEX,
		INSTANCE
	};

	enum class AttributeFormat
	{
		R32_SFLOAT,          // FLOAT
		R64_SFLOAT,          // DOUBLE
		R32G32_SFLOAT,       // VEC2
		R32G32B32_SFLOAT,    // VEC3
		R32G32B32A32_SFLOAT, // VEC4
		R32G32_SINT,         // IVEC2
		R32G32B32A32_UINT,   // UVEC4
	};

	struct BindingDescriptor
	{
		u32 binding;
		u32 stride;
		InputRate inputRate = InputRate::VERTEX;
	};

	struct Attribute
	{
		u32 binding;
		u32 location;
		u32 offset;
		AttributeFormat format;
	};

	struct VertexInputDescriptor
	{
		std::vector<BindingDescriptor> vertexBindings;
		std::vector<Attribute> attributes;
	};
}

namespace tur
{
	enum class PrimitiveTopology
	{
		POINTS,

		LINES,
		LINE_STRIPS,

		TRIANGLES,
		TRIANGLE_STRIPS,
		TRIANGLE_FANS,

		LINES_WITH_ADJACENCY,
		LINE_STRIPS_WITH_ADJACENCY,

		TRIANGLES_WITH_ADJACENCY,
		TRIANGLE_STRIPS_WITH_ADJACENCY,

		PATCHES,
	};

	struct InputAssemblyDescriptor
	{
		PrimitiveTopology topology = PrimitiveTopology::TRIANGLES;
		bool primitiveRestartEnable = false;
	};
}

namespace tur
{
	enum class FrontFace
	{
		COUNTER_CLOCKWISE,
		CLOCKWISE
	};

	enum class CullMode : u32
	{
		NONE = 1 << 0,
		FRONT = 1 << 1,
		BACK = 1 << 2,
		FRONT_AND_BACK = 1 << 3
	};
	inline u32 operator&(CullMode lhs, CullMode rhs)
	{
		return static_cast<u32>(lhs) & static_cast<u32>(rhs);
	}
	inline u32 operator|(CullMode lhs, CullMode rhs)
	{
		return static_cast<u32>(lhs) | static_cast<u32>(rhs);
	}

	enum class PolygonMode
	{
		FILL,
		LINE,
		POINT,
		FILL_RECTANGLE
	};

	struct RasterizerDescriptor
	{
		FrontFace frontFace = FrontFace::COUNTER_CLOCKWISE;
		CullMode cullMode = CullMode::FRONT;
		PolygonMode polygonMode = PolygonMode::FILL;

		float lineWidth = 1.0f;

		bool discardFragments = false;
		bool enableDepthBias = false;
		bool clampDepth = false;
	};
}

namespace tur
{
	using pipeline_handle = handle_type;

	enum class DynamicState
	{
		VIEWPORT,
		SCISSOR
	};

	struct PipelineDescriptor
	{
		VertexInputDescriptor vertexInputStage;
		InputAssemblyDescriptor inputAssemblyStage;
		RasterizerDescriptor rasterizerStage;

		DescriptorSetLayout descriptorSetLayout;

		shader_handle vertexShader = invalid_handle;
		shader_handle tesselationControlShader = invalid_handle;
		shader_handle tesselationEvaluationShader = invalid_handle;
		shader_handle geometryShader = invalid_handle;
		shader_handle fragmentShader = invalid_handle;

		// Viewport & Scissor:
		std::vector<Viewport> viewports = {};
		std::vector<Extent2D> scissors = {};

		std::span<const DynamicState> dynamicStates = {};
	};
}