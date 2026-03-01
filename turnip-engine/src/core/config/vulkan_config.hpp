#pragma once
#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	struct InstanceRequirements
	{
		std::vector<std::string> layers, extensions;

		bool enableDrawing = true;
		bool enableValidationLayers = false;
	};

	struct LogicalDeviceRequirements
	{
		bool customQueueRequirements = false;
		std::vector<std::string> requiredQueueOperations = { "GRAPHICS", "PRESENT" };
		std::vector<std::string> extensions = {};
	};

	struct PhysicalDeviceRequirements
	{
		// Mandatory:
		std::vector<std::string> extensions = {"VK_KHR_dynamic_rendering", "VK_KHR_synchronization2"};

		// Optional:
		bool preferDiscrete = true;
		bool enableMDI = true;
	};

	struct VulkanConfig
	{
		InstanceRequirements instanceRequirements;
		PhysicalDeviceRequirements physicalDeviceRequirements;
		LogicalDeviceRequirements logicalDeviceRequirements;
	};
}