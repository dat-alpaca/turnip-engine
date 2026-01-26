#pragma once
#include <vulkan/vulkan.hpp>
#include "logging/logging.hpp"

// TODO: improve this thing someday.

namespace tur::vulkan
{
	template <typename Object>
	constexpr inline Object check_vk_object(const vk::ResultValue<Object>& result, const char* objectName)
	{
		if (result.result == vk::Result::eSuccess)
			return result.value;

		TUR_LOG_CRITICAL("Failed to create object {}", objectName);
	}

	template <typename Object>
	constexpr inline Object check_vk_result(vk::ResultValue<Object>&& result)
	{
		if (result.result == vk::Result::eSuccess)
			return result.value;

		switch (result.result)
		{
			case vk::Result::eSuccess:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eSuccess, 1));
				break;

			case vk::Result::eNotReady:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eNotReady, 1));
				break;

			case vk::Result::eTimeout:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eTimeout, 1));
				break;

			case vk::Result::eEventSet:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eEventSet, 1));
				break;

			case vk::Result::eEventReset:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eEventReset, 1));
				break;

			case vk::Result::eIncomplete:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eIncomplete, 1));
				break;

			case vk::Result::eErrorOutOfHostMemory:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorOutOfHostMemory, 1));
				break;

			case vk::Result::eErrorOutOfDeviceMemory:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorOutOfDeviceMemory, 1));
				break;

			case vk::Result::eErrorInitializationFailed:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorInitializationFailed, 1));
				break;

			case vk::Result::eErrorDeviceLost:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorDeviceLost, 1));
				break;

			case vk::Result::eErrorMemoryMapFailed:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorMemoryMapFailed, 1));
				break;

			case vk::Result::eErrorLayerNotPresent:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorLayerNotPresent, 1));
				break;

			case vk::Result::eErrorExtensionNotPresent:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorExtensionNotPresent, 1));
				break;

			case vk::Result::eErrorFeatureNotPresent:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorFeatureNotPresent, 1));
				break;

			case vk::Result::eErrorIncompatibleDriver:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorIncompatibleDriver, 1));
				break;

			case vk::Result::eErrorTooManyObjects:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorTooManyObjects, 1));
				break;

			case vk::Result::eErrorFormatNotSupported:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorFormatNotSupported, 1));
				break;

			case vk::Result::eErrorFragmentedPool:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorFragmentedPool, 1));
				break;

			case vk::Result::eErrorUnknown:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorUnknown, 1));
				break;

			case vk::Result::eErrorOutOfPoolMemory:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorOutOfPoolMemory, 1));
				break;

			case vk::Result::eErrorInvalidExternalHandle:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorInvalidExternalHandle, 1));
				break;

			case vk::Result::eErrorFragmentation:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorFragmentation, 1));
				break;

			case vk::Result::eErrorInvalidOpaqueCaptureAddress:
				TUR_LOG_CRITICAL(
					"[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorInvalidOpaqueCaptureAddress, 1)
				);
				break;

			case vk::Result::ePipelineCompileRequired:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(ePipelineCompileRequired, 1));
				break;

			case vk::Result::eErrorSurfaceLostKHR:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorSurfaceLostKHR, 1));
				break;

			case vk::Result::eErrorNativeWindowInUseKHR:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorNativeWindowInUseKHR, 1));
				break;

			case vk::Result::eSuboptimalKHR:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eSuboptimalKHR, 1));
				break;

			case vk::Result::eErrorOutOfDateKHR:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorOutOfDateKHR, 1));
				break;

			case vk::Result::eErrorIncompatibleDisplayKHR:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorIncompatibleDisplayKHR, 1));
				break;

			case vk::Result::eErrorValidationFailedEXT:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorValidationFailedEXT, 1));
				break;

			case vk::Result::eErrorInvalidShaderNV:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorInvalidShaderNV, 1));
				break;

			case vk::Result::eErrorImageUsageNotSupportedKHR:
				TUR_LOG_CRITICAL(
					"[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorImageUsageNotSupportedKHR, 1)
				);
				break;

			case vk::Result::eErrorVideoPictureLayoutNotSupportedKHR:
				TUR_LOG_CRITICAL(
					"[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorVideoPictureLayoutNotSupportedKHR, 1)
				);
				break;

			case vk::Result::eErrorVideoProfileOperationNotSupportedKHR:
				TUR_LOG_CRITICAL(
					"[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorVideoProfileOperationNotSupportedKHR, 1)
				);
				break;

			case vk::Result::eErrorVideoProfileFormatNotSupportedKHR:
				TUR_LOG_CRITICAL(
					"[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorVideoProfileFormatNotSupportedKHR, 1)
				);
				break;

			case vk::Result::eErrorVideoProfileCodecNotSupportedKHR:
				TUR_LOG_CRITICAL(
					"[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorVideoProfileCodecNotSupportedKHR, 1)
				);
				break;

			case vk::Result::eErrorVideoStdVersionNotSupportedKHR:
				TUR_LOG_CRITICAL(
					"[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorVideoStdVersionNotSupportedKHR, 1)
				);
				break;

			case vk::Result::eErrorInvalidDrmFormatModifierPlaneLayoutEXT:
				TUR_LOG_CRITICAL(
					"[Vulkan]: An error has occurred: {}",
					REMOVE_PREFIX(eErrorInvalidDrmFormatModifierPlaneLayoutEXT, 1)
				);
				break;

			case vk::Result::eErrorNotPermittedEXT:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorNotPermittedEXT, 1));
				break;

			case vk::Result::eThreadIdleKHR:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eThreadIdleKHR, 1));
				break;

			case vk::Result::eThreadDoneKHR:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eThreadDoneKHR, 1));
				break;

			case vk::Result::eOperationDeferredKHR:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eOperationDeferredKHR, 1));
				break;

			case vk::Result::eOperationNotDeferredKHR:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eOperationNotDeferredKHR, 1));
				break;

			case vk::Result::eErrorInvalidVideoStdParametersKHR:
				TUR_LOG_CRITICAL(
					"[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorInvalidVideoStdParametersKHR, 1)
				);
				break;

			case vk::Result::eErrorCompressionExhaustedEXT:
				TUR_LOG_CRITICAL(
					"[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eErrorCompressionExhaustedEXT, 1)
				);
				break;

			case vk::Result::eIncompatibleShaderBinaryEXT:
				TUR_LOG_CRITICAL("[Vulkan]: An error has occurred: {}", REMOVE_PREFIX(eIncompatibleShaderBinaryEXT, 1));
				break;
		}

		return {};
	}
}
