#pragma once
#include "common.hpp"

#include "graphics/objects/shader.hpp"
#include "utils/file.hpp"
#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	inline vk::ShaderModule allocate_shader_module(vk::Device device, const ShaderDescriptor& descriptor)
	{
		std::vector<u8> shaderData = read_file_binary(descriptor.filepath);

		vk::ShaderModuleCreateInfo createInfo;
		{
			createInfo.codeSize = shaderData.size();
			createInfo.pCode = reinterpret_cast<const u32*>(shaderData.data());
		}

		vk::ShaderModule shaderModule;
		try
		{
			shaderModule = device.createShaderModule(createInfo);
		}
		catch (vk::SystemError& err)
		{
			TUR_LOG_CRITICAL("Failed to create shader module: {}", err.what());
		}

		return shaderModule;
	}
}