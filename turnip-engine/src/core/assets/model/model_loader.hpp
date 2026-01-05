#pragma once
#include <filesystem>
#include <optional>
#include <tiny_gltf.h>
#include "assets/model/model_asset.hpp"

namespace tur 
{
    std::optional<tinygltf::Model> load_model_task(const std::filesystem::path& filepath);
    
    MeshData extract_mesh_data(const tinygltf::Model& model);

    MetallicRoughnessMaterial extract_texture_data(const tinygltf::Model& model);
}