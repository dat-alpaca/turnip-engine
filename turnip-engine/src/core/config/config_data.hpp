#pragma once
#include <filesystem>

#include "application_config.hpp"
#include "engine_config.hpp"
#include "physics_config.hpp"
#include "vulkan_config.hpp"
#include "window_config.hpp"
#include "worker_config.hpp"

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
	};
}

namespace tur
{
	void initialize_config_data(const std::filesystem::path& filepath);
}