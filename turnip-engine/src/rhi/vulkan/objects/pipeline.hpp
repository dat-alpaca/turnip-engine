#pragma once
#include "graphics/objects/pipeline.hpp"
#include "graphics/types/pipeline_stage.hpp"
#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	constexpr inline vk::FrontFace get_front_face(FrontFace frontFace)
	{
		switch (frontFace)
		{
			case FrontFace::COUNTER_CLOCKWISE:
				return vk::FrontFace::eCounterClockwise;

			case FrontFace::CLOCKWISE:
				return vk::FrontFace::eClockwise;
		}

		TUR_LOG_ERROR("Invalid Front Face: {}. Default: eCounterClockwise", static_cast<int>(frontFace));
		return vk::FrontFace::eCounterClockwise;
	}

	constexpr inline vk::CullModeFlags get_cull_mode(CullMode cullMode)
	{
		switch (cullMode)
		{
			case CullMode::NONE:
				return vk::CullModeFlagBits::eNone;

			case CullMode::FRONT:
				return vk::CullModeFlagBits::eFront;

			case CullMode::BACK:
				return vk::CullModeFlagBits::eBack;

			case CullMode::FRONT_AND_BACK:
				return vk::CullModeFlagBits::eFrontAndBack;
		}

		TUR_LOG_ERROR("Invalid Cull Mode: {}. Default: eFront", static_cast<int>(cullMode));
		return vk::CullModeFlagBits::eFront;
	}

	constexpr inline vk::PolygonMode get_polygon_mode(PolygonMode polygonMode)
	{
		switch (polygonMode)
		{
			case PolygonMode::FILL:
				return vk::PolygonMode::eFill;

			case PolygonMode::LINE:
				return vk::PolygonMode::eLine;

			case PolygonMode::POINT:
				return vk::PolygonMode::ePoint;
		}

		TUR_LOG_ERROR("Invalid Polygon Mode: {}. Default: eFill", static_cast<int>(polygonMode));
		return vk::PolygonMode::eFill;
	}

	constexpr inline vk::PrimitiveTopology get_primitive_topology(PrimitiveTopology primitiveTopology)
	{
		switch (primitiveTopology)
		{
			case PrimitiveTopology::POINTS:
				return vk::PrimitiveTopology::ePointList;

			case PrimitiveTopology::LINES:
				return vk::PrimitiveTopology::eLineList;

			case PrimitiveTopology::LINE_STRIPS:
				return vk::PrimitiveTopology::eLineStrip;

			case PrimitiveTopology::TRIANGLES:
				return vk::PrimitiveTopology::eTriangleList;

			case PrimitiveTopology::TRIANGLE_STRIPS:
				return vk::PrimitiveTopology::eTriangleStrip;

			case PrimitiveTopology::TRIANGLE_FANS:
				return vk::PrimitiveTopology::eTriangleFan;

			case PrimitiveTopology::LINES_WITH_ADJACENCY:
				return vk::PrimitiveTopology::eLineListWithAdjacency;

			case PrimitiveTopology::LINE_STRIPS_WITH_ADJACENCY:
				return vk::PrimitiveTopology::eLineStripWithAdjacency;

			case PrimitiveTopology::TRIANGLES_WITH_ADJACENCY:
				return vk::PrimitiveTopology::eTriangleListWithAdjacency;

			case PrimitiveTopology::TRIANGLE_STRIPS_WITH_ADJACENCY:
				return vk::PrimitiveTopology::eTriangleStripWithAdjacency;

			case PrimitiveTopology::PATCHES:
				return vk::PrimitiveTopology::eTriangleList;
		}

		TUR_LOG_ERROR("Invalid Topology: {}. Default: eTriangleList", static_cast<int>(primitiveTopology));
		return vk::PrimitiveTopology::eTriangleList;
	}
}

namespace tur::vulkan
{
	constexpr inline vk::Format get_attribute_format(AttributeFormat format)
	{
		switch (format)
		{
			case AttributeFormat::R32_SFLOAT:
				return vk::Format::eR32Sfloat;

			case AttributeFormat::R64_SFLOAT:
				return vk::Format::eR64Sfloat;

			case AttributeFormat::R32G32_SFLOAT:
				return vk::Format::eR32G32Sfloat;

			case AttributeFormat::R32G32B32_SFLOAT:
				return vk::Format::eR32G32B32Sfloat;

			case AttributeFormat::R32G32B32A32_SFLOAT:
				return vk::Format::eR32G32B32A32Sfloat;

			case AttributeFormat::R32G32_SINT:
				return vk::Format::eR32G32Sint;

			case AttributeFormat::R32G32B32A32_UINT:
				return vk::Format::eR32G32B32A32Uint;
		}

		TUR_LOG_CRITICAL("Invalid Attribute Format: {}", static_cast<u32>(format));
		return vk::Format::eUndefined;
	}
}

namespace tur::vulkan
{
	constexpr inline vk::PipelineBindPoint get_pipeline_type(PipelineType type)
	{
		switch (type)
		{
			case PipelineType::GRAPHICS:
				return vk::PipelineBindPoint::eGraphics;

			case PipelineType::COMPUTE:
				return vk::PipelineBindPoint::eCompute;
		}

		TUR_LOG_CRITICAL("Invalid Pipeline Type: {}", static_cast<u32>(type));
		return vk::PipelineBindPoint::eGraphics;
	}

	constexpr inline vk::ShaderStageFlags get_pipeline_stages(PipelineStage stage)
	{
		vk::ShaderStageFlags flags;

		if (static_cast<u32>(stage) & static_cast<u32>(PipelineStage::VERTEX_STAGE))
			flags |= vk::ShaderStageFlagBits::eVertex;

		if (static_cast<u32>(stage) & static_cast<u32>(PipelineStage::FRAGMENT_STAGE))
			flags |= vk::ShaderStageFlagBits::eFragment;

		if (static_cast<u32>(stage) & static_cast<u32>(PipelineStage::GRAPHICS_ALL))
			flags |= vk::ShaderStageFlagBits::eAllGraphics;

		return flags;
	}
}

namespace tur::vulkan
{
	struct Pipeline
	{
		PipelineType type;
		vk::Pipeline pipeline;
		vk::PipelineLayout layout;
		vk::DescriptorSetLayout setLayout;
		PipelineDescriptor descriptor;
	};
}