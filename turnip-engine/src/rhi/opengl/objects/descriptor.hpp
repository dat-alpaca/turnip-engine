#pragma once
#include "graphics/objects/buffer.hpp"
#include "graphics/objects/descriptor.hpp"
#include "graphics/objects/texture.hpp"
#include <vector>

namespace tur::gl
{
	struct Descriptor
	{
		handle_type resourceHandle;
		DescriptorType type;
		u32 binding;
	};

	struct DescriptorSet
	{
		std::unordered_map<u32, Descriptor> descriptors;
		descriptor_set_layout_handle layout;
	};

	struct DescriptorSetLayout
	{
		DescriptorSetLayoutDescriptor descriptor;
	};
}