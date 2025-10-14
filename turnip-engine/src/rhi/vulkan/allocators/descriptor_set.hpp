#pragma once
#include "graphics/objects/descriptor.hpp"
#include "rhi/vulkan/objects/descriptor.hpp"
#include "rhi/vulkan/objects/pipeline.hpp"
#include "rhi/vulkan/utils/logger.hpp"

#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	inline vk::DescriptorSetLayout allocate_descriptor_set_layout(vk::Device device, const DescriptorSetLayout& layout)
	{
		std::vector<vk::DescriptorSetLayoutBinding> descriptorBindings;
		for (const auto& [binding, amount, type, stages] : layout.entries)
		{
			vk::DescriptorSetLayoutBinding descriptorBinding = {};
			descriptorBinding.binding = binding;
			descriptorBinding.descriptorType = get_descriptor_type(type);
			descriptorBinding.stageFlags = get_pipeline_stages(stages);
			descriptorBinding.descriptorCount = amount;

			descriptorBindings.push_back(descriptorBinding);
		}

		vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = {};
		descriptorSetLayoutInfo.bindingCount = static_cast<u32>(descriptorBindings.size());
		descriptorSetLayoutInfo.pBindings = nullptr;

		if (descriptorSetLayoutInfo.bindingCount)
			descriptorSetLayoutInfo.pBindings = descriptorBindings.data();

		return check_vk_object(device.createDescriptorSetLayout(descriptorSetLayoutInfo), "DescriptorSetLayout");
	}
}