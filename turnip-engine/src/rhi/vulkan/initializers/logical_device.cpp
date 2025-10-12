#include "logical_device.hpp"

#include "rhi/vulkan/types/queue_family.hpp"
#include "rhi/vulkan/vulkan_constants.hpp"

namespace tur::vulkan
{
	void initialize_logical_device(VulkanState& state, const VulkanConfig& vulkanConfig)
	{
		using queue_family_index = handle_type;

		const auto& logicDeviceReqs = vulkanConfig.logicalDeviceRequirements;
		const auto& requiredQueueOperations = logicDeviceReqs.requiredQueueOperations;
		const auto& queueFamilies = get_queue_families(state.physicalDevice, state.surface);

		// Queue Creation Info:
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		float priority = 1.0f;
		{
			if (logicDeviceReqs.customQueueRequirements)
			{
				/* TODO: Implement */
			}

			for (const auto& [index, _] : queueFamilies)
			{
				vk::DeviceQueueCreateInfo createInfo{vk::DeviceQueueCreateFlags(), index, 1, &priority};
				queueCreateInfos.push_back(createInfo);
			}
		}

		// Layers:
		std::vector<const char*> layers;
		for (const auto& layer : vulkanConfig.instanceRequirements.layers)
			layers.push_back(layer.c_str());

		// Extensions:
		std::vector<const char*> extensions;
		{
			for (const auto& extension : vulkanConfig.physicalDeviceRequirements.extensions)
				extensions.push_back(extension.c_str());

			if (state.requiresDrawing)
				extensions.push_back(SwapchainExtensionName);
		}

		// Features:
		vk::PhysicalDeviceVulkan12Features vulkan12Features;
		vk::PhysicalDeviceVulkan13Features vulkan13Features;
		vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();
		{
			// Required bufffer device address feature
			vulkan12Features.bufferDeviceAddress = true;
			vulkan12Features.pNext = &vulkan13Features;

			const auto& strExtensions = vulkanConfig.physicalDeviceRequirements.extensions;

			if (std::find(strExtensions.begin(), strExtensions.end(), VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)
				!= strExtensions.end())
				vulkan13Features.dynamicRendering = true;

			if (std::find(strExtensions.begin(), strExtensions.end(), VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME)
				!= strExtensions.end())
				vulkan13Features.synchronization2 = true;
		}

		// Device creation:
		vk::DeviceCreateInfo deviceInfo = vk::DeviceCreateInfo(
			vk::DeviceCreateFlags(), static_cast<u32>(queueCreateInfos.size()), queueCreateInfos.data(),
			static_cast<u32>(layers.size()), layers.data(), static_cast<u32>(extensions.size()), extensions.data(),
			&deviceFeatures, &vulkan12Features
		);

		try
		{
			state.logicalDevice = state.physicalDevice.createDevice(deviceInfo);
		}
		catch (const std::exception& err)
		{
			TUR_LOG_ERROR("Failed to create logical device: {}", err.what());
		}

		// Queue Creation:
		// TODO: improve this mess
		queue_family_index graphicsIndex, presentIndex;
		bool graphicsSet = false, presentSet = false;
		for (const auto& [queueFamilyIndex, operation] : queueFamilies)
		{
			if ((operation & QueueOperation::GRAPHICS) && !graphicsSet)
			{
				graphicsIndex = queueFamilyIndex;
				graphicsSet = true;
			}
			if ((operation & QueueOperation::PRESENT) && !presentSet)
			{
				presentIndex = queueFamilyIndex;
				presentSet = true;
			}
		}

		vk::Queue queue = state.logicalDevice.getQueue(graphicsIndex, 0);
		QueueOperation operation = (QueueOperation)(QueueOperation::GRAPHICS | QueueOperation::TRANSFER);
		state.queueList.register_queue({queue, operation, graphicsIndex});

		queue = state.logicalDevice.getQueue(presentIndex, 0);
		state.queueList.register_queue({queue, QueueOperation::PRESENT, presentIndex});
	}
}