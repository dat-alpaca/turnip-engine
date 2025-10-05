#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rhi/vulkan/requirements/instance.hpp"
#include "rhi/vulkan/requirements/logical_device.hpp"
#include "rhi/vulkan/requirements/physical_device.hpp"

namespace tur::vulkan
{
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InstanceRequirements, layers, extensions, enableDrawing, enableValidationLayers);

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PhysicalDeviceRequirements, extensions, preferDiscrete);

	// Logical Device:
	inline void to_json(nlohmann::json& json, const LogicalDeviceRequirements& requirements)
	{
		json["customQueueRequirements"] = requirements.customQueueRequirements;
		json["requiredQueueOperations"] = nlohmann::json::array();

		for (const auto& operation : requirements.requiredQueueOperations)
		{
			for (const auto& usageString : get_queue_operation_strings(operation))
				json["requiredQueueOperations"].push_back(usageString);
		}
	}

	inline void from_json(const nlohmann::json& json, LogicalDeviceRequirements& requirements)
	{
		requirements.customQueueRequirements = json["customQueueRequirements"];

		for (const auto& operation : json["requiredQueueOperations"])
			requirements.requiredQueueOperations.push_back(get_queue_operation_from_string(operation));
	}

	struct VulkanConfig
	{
		InstanceRequirements instanceRequirements;
		PhysicalDeviceRequirements physicalDeviceRequirements;
		LogicalDeviceRequirements logicalDeviceRequirements;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(VulkanConfig, instanceRequirements, physicalDeviceRequirements,
									   logicalDeviceRequirements);
	};
}