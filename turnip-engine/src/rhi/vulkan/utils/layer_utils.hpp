#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>

#include "validation_results.hpp"

namespace tur::vulkan
{
	inline bool check_layer_supported(const std::vector<vk::LayerProperties>& availableLayers, const char* layerName)
	{
		if (!layerName)
			return false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
				return true;
		}

		return false;
	}

	inline ValidationResults validate_layers(const std::vector<vk::LayerProperties>& supportedLayers,
											 const std::vector<const char*>& layers)
	{

		ValidationResults results;
		results.success = true;

		for (const char* layer : layers)
		{
			if (!check_layer_supported(supportedLayers, layer))
			{
				results.unsupported.push_back(layer);
				results.success = false;
			}
		}

		return results;
	}
}