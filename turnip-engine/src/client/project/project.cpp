#include "client/project/project.hpp"
#include "assets/asset_library.hpp"
#include <filesystem>
#include <vector>

#include "assets/texture/texture_asset.hpp"
#include "client/asset_serialization.hpp"
#include "client/scene_serialization.hpp"
#include "utils/json/json_file.hpp"

namespace tur
{
    Project::Project(const std::filesystem::path& folderPath)
        : folderPath(folderPath)
    {
        initialize(folderPath);
    }

    void Project::initialize(const std::filesystem::path& folderPath)
    {
        this->folderPath = folderPath;

        if(!std::filesystem::exists(folderPath))
            std::filesystem::create_directory(folderPath);

        if(!std::filesystem::is_directory(folderPath))
            TUR_LOG_CRITICAL("Invalid folderpath passed to Project");
    
        name = folderPath.filename();
        mainFilepath = get_project_filepath(name + Project::ProjectFileExtension);

        read_project();
    }

    void Project::save()
    {
        nlohmann::json projectObject;
        projectObject["name"] = name;
            
        // Scenes:
        projectObject["scenes"] = nlohmann::json::array();
        for(const auto& scene : mSceneFilepaths)
            projectObject["scenes"].push_back(scene.string());
        
        std::filesystem::path filepath = get_project_filepath(name + Project::ProjectFileExtension);
        json_write_file(filepath, projectObject);

        // Assets:
        filepath = get_project_filepath("asset_library.asses");
        json_write_file(filepath, assetLibraryObject);
    }

    void Project::save_asset_library(NON_OWNING AssetLibrary* library)
    {
        assetLibraryObject = serialize_asset_library(library);
    }

    void Project::add_scene(NON_OWNING Scene* scene, const std::filesystem::path& filepath)
    {
        serialize_scene(scene, get_project_filepath(filepath));
        mSceneFilepaths.push_back(filepath);
    }
}

namespace tur
{
    std::vector<TextureAsset> Project::get_textures() const
    {
        return assetLibraryObject["textures"];
    }

    std::vector<AudioAsset> Project::get_audios() const 
    { 
        return assetLibraryObject["audios"]; 
    }
}

namespace tur 
{
    void Project::read_project()
    {
        std::filesystem::path mainFilepath = get_project_filepath(folderPath.filename().string() + Project::ProjectFileExtension);
            
        if(!std::filesystem::exists(mainFilepath))
        {
            json_write_file(mainFilepath, {});
            return;
        }
        
        // Main project:
        nlohmann::json projectObject = json_parse_file(mainFilepath);
        name = projectObject["name"];

        // Assets:
        std::filesystem::path assetLibraryFilepath = get_project_filepath(std::string("asset_library.asses"));
        assetLibraryObject = deserialize_asset_library(assetLibraryFilepath);
        
        // Scenes:
        if(projectObject.contains("scenes") && projectObject["scenes"].is_array())
        {
            for(const auto& object : projectObject["scenes"])
            {
                if(object.is_string())
                    mSceneFilepaths.push_back(object);
            }
        }
    }
}