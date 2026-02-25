#pragma once
#include <json.hpp>
#include "assets/asset_library.hpp"

#include "utils/json/json_file.hpp"
#include "bridge/serialization/asset.hpp"

namespace tur
{
    inline nlohmann::json serialize_asset_library(NON_OWNING AssetLibrary* library)
    {
        nlohmann::json object = nlohmann::json::object();

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
        for(const auto& mesh : library->get_mesh_assets())
            object["meshes"].push_back(mesh);

        object["cubemaps"] = nlohmann::json::array(); 
        for(const auto& cubemap : library->get_cubemap_assets())
            object["cubemaps"].push_back(cubemap);

        return object;
    }

    inline nlohmann::json deserialize_asset_library(const std::filesystem::path& filepath)
    {
        return read_json(filepath);
    }
}