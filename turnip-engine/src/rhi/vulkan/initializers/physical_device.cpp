#include "physical_device.hpp"

#include "rhi/vulkan/types/queue_family.hpp"
#include "rhi/vulkan/utils/extension_utils.hpp"
#include "rhi/vulkan/utils/layer_utils.hpp"
#include "rhi/vulkan/utils/logger.hpp"
#include "rhi/vulkan/utils/swapchain_features.hpp"
#include "rhi/vulkan/vulkan_constants.hpp"

namespace tur::vulkan
{
	void initialize_physical_device(VulkanState& state, const PhysicalDeviceRequirements& requirements)
	{
		auto physicalDevices = check_vk_object(state.instance.enumeratePhysicalDevices(), "PhysicalDevices");
		if (physicalDevices.empty())
			TUR_LOG_CRITICAL("No valid vulkan physical devices found.");

		std::vector<const char*> extensions;
		for (const auto& extension : requirements.extensions)
			extensions.push_back(extension.c_str());

		if (state.requiresDrawing)
			extensions.push_back(SwapchainExtensionName);

		std::vector<vk::PhysicalDevice> suitableDevices;
		{
			for (const auto& device : physicalDevices)
			{
				auto deviceName = device.getProperties().deviceName.data();

				// Drawing Queues, Extensions & Swapchain:
				auto supportedQueueFeatures = get_queue_family_supported_features(device, state.surface);
				if (state.requiresDrawing)
				{
					if (!(supportedQueueFeatures & QueueOperation::GRAPHICS))
						continue;

					if (!(supportedQueueFeatures & QueueOperation::PRESENT))
						continue;

					auto swapchainCapabilities = fetch_swapchain_features(device, state.surface);
					if (swapchainCapabilities.availableSurfaceFormats.empty())
						continue;

					if (swapchainCapabilities.presentModes.empty())
						continue;
				}

				// Extension Support:
				auto extensionSupport = validate_extensions(
					check_vk_object(device.enumerateDeviceExtensionProperties(), "DeviceExtensionProps"), extensions
				);
				if (!extensionSupport.success)
				{
					TUR_LOG_INFO("{} does not support the following extensions: ", deviceName);

					for (const auto& unsupported : extensionSupport.unsupported)
						TUR_LOG_INFO(" * {}", unsupported);
				}

				suitableDevices.push_back(device);
			}
		}

		if (suitableDevices.size() == 0)
			TUR_LOG_CRITICAL("No suitable devices were found.");

		vk::PhysicalDevice chosenDevice = suitableDevices[0];
		{
			for (const auto& device : suitableDevices)
			{
				const auto& properties = device.getProperties();

				if (requirements.preferDiscrete && properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
					chosenDevice = device;
			}
		}

		state.physicalDevice = chosenDevice;
	}
}