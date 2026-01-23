#include "instance.hpp"

#include "rhi/vulkan/utils/extension_utils.hpp"
#include "rhi/vulkan/utils/layer_utils.hpp"
#include "rhi/vulkan/utils/logger.hpp"
#include "rhi/vulkan/vulkan_constants.hpp"

#include "platform/platform.hpp"

namespace tur::vulkan
{
	static const char* get_message_severity_name(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity)
	{
		switch (messageSeverity)
		{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				return "VERBOSE";

			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				return "ERROR";

			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				return "WARNING";

			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				return "INFO";

			default:
				return "UNKNOWN";
		}
	}

	static const char* get_message_type_name(VkDebugUtilsMessageTypeFlagsEXT messageType)
	{
		if (messageType == 7)
			return "General | Validation | Performance";

		if (messageType == 6)
			return "Validation | Performance";

		if (messageType == 5)
			return "General | Performance";

		if (messageType == 4)
			return "Performance";

		if (messageType == 3)
			return "General | Validation";

		if (messageType == 2)
			return "Validation";

		if (messageType == 1)
			return "General";

		return "Unknown";
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL default_debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData
	)
	{
		auto severity = get_message_severity_name(messageSeverity);
		auto type = get_message_type_name(messageType);

		switch (messageSeverity)
		{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				TUR_LOG_DEBUG("[Vulkan: {}] [{}]: {}", severity, type, pCallbackData->pMessage);
				break;

			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				TUR_LOG_INFO("[Vulkan: {}] [{}]: {}", severity, type, pCallbackData->pMessage);
				break;

			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				TUR_LOG_WARN("[Vulkan: {}] [{}]: {}", severity, type, pCallbackData->pMessage);
				break;

			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				TUR_LOG_ERROR("[Vulkan: {}] [{}]: {}", severity, type, pCallbackData->pMessage);
				break;

			default:
				return VK_FALSE;
		}

		return VK_FALSE;
	}

	static void destroy_messenger_ext(
		vk::Instance instance, vk::DebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator
	)
	{
		auto func =
			(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

		if (func)
			func(instance, debugMessenger, pAllocator);
	}
}

namespace tur::vulkan
{
	void initialize_instance(VulkanState& state, const ConfigData& configData)
	{
		auto& appConfig = configData.applicationSpecs;
		auto& engineConfig = configData.engineSpecs;
		auto& windowingCaps = configData.windowingCapabilities;

		auto& instanceConfig = configData.vulkanConfig.instanceRequirements;

		state.apiVersion =
			VK_MAKE_API_VERSION(windowingCaps.variant, windowingCaps.major, windowingCaps.minor, windowingCaps.patch);
		state.requiresDrawing = instanceConfig.enableDrawing;
		state.validationEnabled = instanceConfig.enableValidationLayers;

		// Application information:
		vk::ApplicationInfo applicationInformation = vk::ApplicationInfo(
			appConfig.applicationName.c_str(),
			VK_MAKE_API_VERSION(appConfig.variant, appConfig.major, appConfig.minor, appConfig.patch), TUR_ENGINE_NAME,
			VK_MAKE_API_VERSION(engineConfig.variant, engineConfig.major, engineConfig.minor, engineConfig.patch),
			state.apiVersion
		);

		// Extensions & Layers:
		std::vector<const char*> layers;
		for (const auto& layer : instanceConfig.layers)
			layers.push_back(layer.c_str());

		std::vector<const char*> extensions;
		for (const auto& extension : instanceConfig.extensions)
			extensions.push_back(extension.c_str());

		if (state.validationEnabled)
		{
			layers.push_back(ValidationLayerName);
			extensions.push_back(DebugUtilsExtensionName);
		}

		std::vector<vk::ExtensionProperties> supportedExtensions =
			check_vk_object(vk::enumerateInstanceExtensionProperties(), "InstanceExtensionProps");
		if (state.requiresDrawing)
		{
			extensions.push_back(SurfaceExtensionName);

			auto& binder = tur::platform::WindowingSystem::get_binder();
			auto availableExtensionNames = binder.get_vulkan_extension_platform_surface_names();
			for (const char* extensionName : availableExtensionNames)
				extensions.push_back(extensionName);
		}

		auto validationResults =
			validate_layers(check_vk_object(vk::enumerateInstanceLayerProperties(), "InstanceLayerProps"), layers);
		if (!validationResults.success)
		{
			TUR_LOG_ERROR("[Vulkan]: Unsupported instance layers:");
			for (const auto& layer : validationResults.unsupported)
				TUR_LOG_ERROR(" * {}", layer);

			// TUR_LOG_CRITICAL("Gracefully handling unsupported layers.");
		}

		validationResults = validate_extensions(supportedExtensions, extensions);
		if (!validationResults.success)
		{
			TUR_LOG_ERROR("[Vulkan]: Unsupported instance extensions:");
			for (const auto& extension : validationResults.unsupported)
				TUR_LOG_ERROR(" * {}", extension);

			// TUR_LOG_CRITICAL("Gracefully handling unsupported extensions.");
		}

		// Instance:
		vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo(
			vk::InstanceCreateFlags(), &applicationInformation, static_cast<uint32_t>(layers.size()), layers.data(),
			static_cast<uint32_t>(extensions.size()), extensions.data()
		);

		state.instance = check_vk_result(vk::createInstance(createInfo));

		// Messenger Creation:
		if (!state.validationEnabled)
			return;

		vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
		{
			debugCreateInfo.flags = {};
			debugCreateInfo.setMessageSeverity(DefaultDebugMessageSeverity);
			debugCreateInfo.setMessageType(DefaultDebugMessageType);
			debugCreateInfo.setPfnUserCallback(default_debug_callback);
		};
	}

	void destroy_instance_messenger(VulkanState& state)
	{
		destroy_messenger_ext(state.instance, state.debugMessenger, nullptr);
	}
}