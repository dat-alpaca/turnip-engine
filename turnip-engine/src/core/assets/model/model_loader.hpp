#pragma once
#include <filesystem>
#include <optional>
#include <tiny_gltf.h>
#include "assets/model/model_asset.hpp"

namespace tur 
{
    inline std::optional<tinygltf::Model> load_model_task(const std::filesystem::path& filepath)
    {
        if (!std::filesystem::exists(filepath))
		{
			TUR_LOG_ERROR("Failed to load model: '{}'. File does not exist.", filepath.string());
			return std::nullopt;
		}

        tinygltf::Model model;
        tinygltf::TinyGLTF loader;

        std::string error;
        std::string warning;
        bool returnValue = loader.LoadASCIIFromFile(&model, &error, &warning, filepath.string());

        if (!warning.empty())
            TUR_LOG_WARN("", warning);

        if (!error.empty())
        {
            TUR_LOG_ERROR("Failed to load: {}. {}", filepath.string(), error);
            return std::nullopt;
        }

        if(!returnValue)
        {
            TUR_LOG_ERROR("Failed to parse: {}", filepath.string());   
            return std::nullopt;
        }
        
        return model;
    }
    
    inline std::vector<std::string> extract_model_textures_task(const tinygltf::Model& model)
    {
        MaterialInfo materialInfo = extract_texture_data(model);

        // TODO: get other textures
        return { materialInfo.albedoFilepath };
    }
}