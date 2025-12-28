#pragma once
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include "assert/assert.hpp"
#include "graphics/objects/descriptor.hpp"

#include "rhi/vulkan/objects/descriptor.hpp"
#include "rhi/vulkan/objects/pipeline.hpp"
#include "rhi/vulkan/utils/logger.hpp"

namespace tur::vulkan
{
	inline vk::DescriptorPool allocate_descriptor_pool(vk::Device device, const DescriptorSetLayoutDescriptor& layout, u32 maxSets)
	{
		TUR_ASS(device);
		TUR_ASS(maxSets > 0);

		std::vector<vk::DescriptorPoolSize> poolSizes;
		for (const auto& entry : layout.entries)
		{
			vk::DescriptorPoolSize poolSize = {};
			poolSize.descriptorCount = entry.amount;
			poolSize.type = get_descriptor_type(entry.type);
			poolSizes.push_back(poolSize);
		}

		vk::DescriptorPoolCreateInfo poolInfo = {};
		{
			poolInfo.poolSizeCount = poolSizes.size();
			poolInfo.pPoolSizes = poolSizes.data();
			poolInfo.maxSets = maxSets;
		}

		return check_vk_object(device.createDescriptorPool(poolInfo), "DescriptorPool");
	}

	inline vk::DescriptorSetLayout allocate_set_layout(vk::Device device, const DescriptorSetLayoutDescriptor& layout)
	{
		TUR_ASS(device);

		std::vector<vk::DescriptorSetLayoutBinding> bindings;
		for (const auto& entry : layout.entries)
		{
			vk::DescriptorSetLayoutBinding binding = {};
			{
				binding.binding = entry.binding;
				binding.descriptorCount = entry.amount;
				binding.descriptorType = get_descriptor_type(entry.type);
				binding.stageFlags = get_pipeline_stages(entry.stage);
				binding.pImmutableSamplers = nullptr;
			}
			bindings.push_back(binding);
		}

		vk::DescriptorSetLayoutCreateInfo createInfo = {};
		{
			createInfo.pBindings = bindings.data();
			createInfo.bindingCount = bindings.size();
		}

		return check_vk_object(device.createDescriptorSetLayout(createInfo), "DescriptorSetLayout");
	}

	inline vk::DescriptorSet
	allocate_set_single_layout(vk::Device device, vk::DescriptorPool pool, vk::DescriptorSetLayout layout)
	{
		vk::DescriptorSetAllocateInfo allocateInfo = {};
		{
			allocateInfo.descriptorPool = pool;
			allocateInfo.descriptorSetCount = 1;
			allocateInfo.pSetLayouts = &layout;
		}

		return check_vk_object(device.allocateDescriptorSets(allocateInfo), "DescriptorSet").front();
	}
}