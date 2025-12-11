#pragma once
#include "common.hpp"
#include "graphics/types/pipeline_stage.hpp"
#include <span>

namespace tur
{
	using descriptor_handle = handle_type;

	enum class DescriptorType
	{
		INVALID = 0,
		COMBINED_IMAGE_SAMPLER,
		UNIFORM_BUFFER,
		STORAGE_BUFFER,
	};
}

namespace tur
{
	using descriptor_set_layout_handle = handle_type;
	using descriptor_set_handle = handle_type;

	struct DescriptorSetLayoutEntry
	{
		u32 binding;
		u32 amount;
		DescriptorType type;
		PipelineStage stage;
	};

	struct DescriptorSetLayoutDescriptor
	{
		std::span<const DescriptorSetLayoutEntry> entries;
	};
}