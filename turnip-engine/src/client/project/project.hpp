#pragma once
#include <filesystem>
#include <vector>
#include <string>

#include "assets/asset_library.hpp"
#include "client/asset_serialization.hpp"
#include "client/scene_serialization.hpp"
#include "logging/logging.hpp"
#include "memory/memory.hpp"
#include "nlohmann/json_fwd.hpp"
#include "utils/json/json_file.hpp"

namespace tur
{
    struct Project
    {
    public:
        Project(const std::filesystem::path& folderPath)
        {
            initialize(folderPath);
        }

        Project() = default;

    public:
        void save()
        {
            nlohmann::json projectObject;
            projectObject["name"] = name;
            
            // Scenes:
            projectObject["scenes"] = nlohmann::json::array();
            for(const auto& scene : mSceneFilepaths)
                projectObject["scenes"].push_back(scene.string());

            std::filesystem::path filepath = folderPath / (name + Project::ProjectFileExtension);
            json_write_file(filepath, projectObject);

            // Assets:
            filepath = folderPath / std::string("asset_library.asses");
            json_write_file(filepath, assetLibraryObject);
        }

        void save_asset_library(NON_OWNING AssetLibrary* library)
        {
            assetLibraryObject = serialize_asset_library(library);
        }

    public:
        void initialize(const std::filesystem::path& folderPath)
        {
            if(!std::filesystem::exists(folderPath))
                std::filesystem::create_directory(folderPath);

            if(!std::filesystem::is_directory(folderPath))
                TUR_LOG_CRITICAL("Invalid folderpath passed to Project");
    
            this->folderPath = folderPath;
            name = folderPath.filename();
            mainFilepath = folderPath / (name + Project::ProjectFileExtension);

            read_project();
        }

        void add_scene(NON_OWNING Scene* scene, const std::filesystem::path& filepath)
        {
            serialize_scene(scene, folderPath / filepath);
            mSceneFilepaths.push_back( filepath);
        }

    private:
        void read_project()
        {
            std::filesystem::path mainFilepath = folderPath / (folderPath.filename().string() + Project::ProjectFileExtension);
            
            if(!std::filesystem::exists(mainFilepath))
            {
                json_write_file(mainFilepath, {});
                return;
            }
            
            // Main project:
            nlohmann::json projectObject = json_parse_file(mainFilepath);
            name = projectObject["name"];

            // Scenes:
            if(projectObject.contains("scenes") && projectObject["scenes"].is_array())
            {
                for(const auto& object : projectObject["scenes"])
                {
                    if(object.is_string())
                        mSceneFilepaths.push_back(object);
                }
            }

            // Assets:
            std::filesystem::path assetLibraryFilepath = folderPath / std::string("asset_library.asses");
            assetLibraryObject = deserialize_asset_library(assetLibraryFilepath);
        }

    public:
        std::filesystem::path mainFilepath;
        std::filesystem::path folderPath;
        nlohmann::json assetLibraryObject;
        std::string name;

    private:
        std::vector<std::filesystem::path> mSceneFilepaths;
        static constexpr inline const char* ProjectFileExtension = ".tur";
    };
}