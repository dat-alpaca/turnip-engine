#pragma once
#include "graphics/objects/shader.hpp"
#include "utils/file.hpp"

#include "rhi/vulkan/utils/logger.hpp"
#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	inline vk::ShaderModule allocate_shader_module(vk::Device device, const ShaderDescriptor& descriptor)
	{
		TUR_ASS(device);
		TUR_ASS(descriptor.type != ShaderType::UNKNOWN);

		std::vector<u8> shaderData = read_file_binary(descriptor.filepath);
		vk::ShaderModuleCreateInfo createInfo;
		{
			createInfo.codeSize = shaderData.size();
			createInfo.pCode = reinterpret_cast<const u32*>(shaderData.data());
		}

		return check_vk_object(device.createShaderModule(createInfo), "ShaderModule");
	}
}