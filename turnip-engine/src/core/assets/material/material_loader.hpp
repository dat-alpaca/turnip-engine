#pragma once
#include "assets/material/material_asset.hpp"
#include "bridge/serialization/graphics.hpp"

#include "logging/logging.hpp"
#include "utils/json/json_file.hpp"
#include <filesystem>
#include <string>

namespace tur
{
	using material_asset_opt = std::optional<MaterialAsset>;

	inline material_asset_opt load_material_task(const std::filesystem::path& filepath)
	{
		if (!std::filesystem::exists(filepath))
		{
			TUR_LOG_ERROR("Failed to load material: '{}'. File does not exist.", filepath.string());
			return std::nullopt;
		}

		auto json = read_json(filepath);
		MaterialAsset asset = {};
		
		/* Basic */
		if (!json.contains("vertexShader") || !json["vertexShader"].is_string())
			TUR_LOG_CRITICAL("Attempted to parse invalid material asset: invalid vertex shader");

		if (!json.contains("fragmentShader") || !json["fragmentShader"].is_string())
			TUR_LOG_CRITICAL("Attempted to parse invalid material asset: invalid fragment shader");

		asset.vertexShaderFilepath = json["vertexShader"].get<std::string>();
		if (!std::filesystem::exists(asset.vertexShaderFilepath))
			TUR_LOG_CRITICAL("Attempted to parse invalid material asset: vertex shader not found");

		asset.fragmentShaderFilepath = json["fragmentShader"].get<std::string>();
		if (!std::filesystem::exists(asset.fragmentShaderFilepath))
			TUR_LOG_CRITICAL("Attempted to parse invalid material asset: vertex shader not found");

		/* Tesselation */
		if (json.contains("tessControlShader") && json["tessControlShader"].is_string())
		{
			asset.tesselationControlShader = json.at("tessControlShader").get<std::string>();

			if (!std::filesystem::exists(asset.tesselationControlShader))
				TUR_LOG_CRITICAL("Attempted to parse invalid material asset: tess control shader not found");
		}

		if (json.contains("tesselationEvalShader") && json["tesselationEvalShader"].is_string())
		{
			asset.tesselationEvaluationShader = json.at("tesselationEvaluationShader").get<std::string>();
			if (!std::filesystem::exists(asset.tesselationEvaluationShader))
				TUR_LOG_CRITICAL("Attempted to parse invalid material asset: tess eval shader not found");
		}

		if (json.contains("geometryShader") && json["geometryShader"].is_string())
		{
			asset.geometryShader = json.at("geometryShader").get<std::string>();

			if (!std::filesystem::exists(asset.geometryShader))
				TUR_LOG_CRITICAL("Attempted to parse invalid material asset: geometry shader not found");
		}

		if (json.contains("inputAssembly") && json["inputAssembly"].is_string())
			asset.inputAssemblyStage = json.at("inputAssembly").get<InputAssemblyDescriptor>();

		if (json.contains("rasterizer") && json["rasterizer"].is_string())
			asset.rasterizerStage = json.at("rasterizer").get<RasterizerDescriptor>();

		if (json.contains("depth") && json["depth"].is_string())
			asset.depthDescriptor = json.at("depth").get<DepthDescriptor>();

		if (json.contains("blend") && json["blend"].is_string())
			asset.blendDescriptor = json.at("blend").get<BlendDescriptor>();

		return asset;
	}
}