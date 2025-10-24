#pragma once
#include <string>
#include <vector>

namespace tur::vulkan
{
	struct PhysicalDeviceRequirements
	{
		// Mandatory:
		std::vector<std::string> extensions = {"VK_KHR_dynamic_rendering", "VK_KHR_synchronization2"};

		// Optional:
		bool preferDiscrete = true;
	};
}