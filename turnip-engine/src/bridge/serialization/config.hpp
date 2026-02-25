#pragma once
#include <json.hpp>
#include "core.hpp"
#include "config/config_data.hpp"

/* Engine */
namespace tur
{
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		ApplicationSpecification, 
		applicationName, major, minor, patch, variant
	);

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		EngineSpecification, 
		major, minor, patch, variant
	);

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		PhysicsConfig,
		gravityAcceleration, fixedTimeStep, substepCount
	);

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		WorkerConfig, 
		threadAmount
	);
}


/* Vulkan */
namespace tur::vulkan
{
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		InstanceRequirements, 
		layers, extensions, enableDrawing, enableValidationLayers
	);

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		PhysicalDeviceRequirements, 
		extensions, preferDiscrete
	);

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		LogicalDeviceRequirements, 
		customQueueRequirements, requiredQueueOperations, extensions
	);

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		VulkanConfig, 
		instanceRequirements, physicalDeviceRequirements, logicalDeviceRequirements
	);
}


/* Window */
namespace tur
{
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		WindowingCapabilities, 
		major, minor, patch, variant,
		scaleToMonitor, srgbCapable, resizable, startsVisible
	);

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		WindowProperties, 
		title, dimensions, position, minSize, maxSize, startsMaximized
	);
}


/* ConfigData */
namespace tur
{
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		ConfigData, 
		engineSpecs, applicationSpecs,
		windowProperties, windowingCapabilities, 
		vulkanConfig, workerConfig, physicsConfig
	);
}
