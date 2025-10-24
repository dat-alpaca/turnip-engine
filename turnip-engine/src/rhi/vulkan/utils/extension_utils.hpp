#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>

#include "validation_results.hpp"

namespace tur::vulkan
{
	inline bool check_extension_support(const std::vector<vk::ExtensionProperties>& availableExtensions,
										const char* extensionName)
	{
		if (!extensionName)
			return false;

		for (const auto& extensionProperties : availableExtensions)
		{
			if (strcmp(extensionName, extensionProperties.extensionName) == 0)
				return true;
		}

		return false;
	}

	inline ValidationResults validate_extensions(const std::vector<vk::ExtensionProperties>& supportedExtensions,
												 const std::vector<const char*>& extensions)
	{
		ValidationResults results;
		results.success = true;

		for (const char* extension : extensions)
		{
			if (!check_extension_support(supportedExtensions, extension))
			{
				results.unsupported.push_back(extension);
				results.success = false;
			}
		}

		return results;
	}
}