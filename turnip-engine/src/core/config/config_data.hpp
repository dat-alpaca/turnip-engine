#pragma once
#include <filesystem>
#include <vulkan/vulkan.hpp>

#include "common.hpp"
#include "graphics/graphics_api.hpp"

#include "application_config.hpp"
#include "engine_config.hpp"
#include "physics_config.hpp"
#include "vulkan_config.hpp"
#include "window_config.hpp"
#include "worker_config.hpp"

#include "utils/json/json_file.hpp"

namespace tur
{
	struct ConfigData
	{
		EngineSpecification engineSpecs;
		ApplicationSpecification applicationSpecs;
		WindowProperties windowProperties;
		WindowingCapabilities windowingCapabilities;
		vulkan::VulkanConfig vulkanConfig;
		WorkerConfig workerConfig;
		PhysicsConfig physicsConfig;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(
			ConfigData, engineSpecs, applicationSpecs, windowProperties, windowingCapabilities,
			vulkanConfig, workerConfig, physicsConfig
		);
	};

	inline void initialize_config_data(const std::filesystem::path& filepath)
	{
		if (std::filesystem::exists(filepath))
			return;

		std::ofstream file(filepath.string());

		ConfigData defaultData;
		json_write_file(filepath, defaultData);
	}
}