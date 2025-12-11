#pragma once
#include "graphics/objects/descriptor.hpp"
#include <vulkan/vulkan.hpp>

// TODO:

namespace tur::vulkan
{
	constexpr inline vk::DescriptorType get_descriptor_type(DescriptorType type)
	{
		switch (type)
		{
			case DescriptorType::COMBINED_IMAGE_SAMPLER:
				return vk::DescriptorType::eCombinedImageSampler;

			case DescriptorType::STORAGE_BUFFER:
				return vk::DescriptorType::eStorageBuffer;
			
			case DescriptorType::UNIFORM_BUFFER:
				return vk::DescriptorType::eUniformBuffer;
		}

		TUR_LOG_CRITICAL("Invalid Descriptor Type: {}", static_cast<u32>(type));
		return {};
	}
}

namespace tur::vulkan
{
	struct DescriptorSetLayout
	{
		DescriptorSetLayoutDescriptor layoutDescriptor;
		vk::DescriptorSetLayout layout;
		vk::DescriptorPool pool;
	};

	struct DescriptorSet
	{
		vk::DescriptorSet set;
		descriptor_set_layout_handle layoutHandle;
	};
}