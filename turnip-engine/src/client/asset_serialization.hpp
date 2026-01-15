#pragma once
#include "assets/asset.hpp"
#include "assets/asset_library.hpp"
#include "assets/audio/audio_asset.hpp"
#include "assets/mesh/mesh_asset.hpp"
#include "assets/texture/texture_options.hpp"

#include "nlohmann/json_fwd.hpp"
#include "utils/json/json_file.hpp"
#include <filesystem>

namespace tur
{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AssetMetadata, filepath, uuid, type, lifetime);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TextureOptions, dataFormat, floatTexture, layerWidth, layerHeight,
        depth, mipLevels, samples, arrayLayers, format, type,
        wrapS, wrapT, wrapR, minFilter, magFilter,
        tiling, generateMipmaps, isTextureArray
    );
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TextureAsset, metadata, width, height, channels, options);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AudioAsset, metadata);

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MeshData, vertices, indices);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MetallicRoughnessMaterialData, baseColorFactor, albedoTexture, normalTexture, metallicTexture, roughnessTexture);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MeshAsset, metadata);
    
    inline nlohmann::json serialize_asset_library(NON_OWNING AssetLibrary* library)
    {
        nlohmann::json object;

        object["textures"] = nlohmann::json::array();
        for(const auto& texture : library->get_texture_assets())
        {
            if(texture.metadata.filepath == "internal/default_texture")
                continue;

            object["textures"].push_back(texture);
        }

        object["audios"] = nlohmann::json::array(); 
        for(const auto& audio : library->get_audio_assets())
            object["audios"].push_back(audio);

        object["meshes"] = nlohmann::json::array(); 
        for(const auto& audio : library->get_mesh_assets())
            object["meshes"].push_back(audio);

        return object;
    }

    inline nlohmann::json deserialize_asset_library(const std::filesystem::path& filepath)
    {
        return json_parse_file(filepath);
    }
}