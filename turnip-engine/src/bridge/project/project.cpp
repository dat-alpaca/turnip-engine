#include "bridge/project/project.hpp"
#include "assets/asset_library.hpp"
#include <filesystem>
#include <vector>

#include "assets/font/font_asset.hpp"
#include "assets/texture/texture_asset.hpp"
#include "bridge/serialization/asset_library.hpp"
#include "bridge/serialization/scene.hpp"

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
        write_json(filepath, projectObject);

        // Assets:
        filepath = get_project_filepath("asset_library.asses");
        write_json(filepath, assetLibraryObject);
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
        if(assetLibraryObject.contains("textures"))
            return assetLibraryObject["textures"];
    
        return {};
    }

    std::vector<AudioAsset> Project::get_audios() const
    {
        if(assetLibraryObject.contains("audios"))
            return assetLibraryObject["audios"];
    
        return {};
    }

    std::vector<MeshAsset> Project::get_meshes() const
    {
        if(assetLibraryObject.contains("meshes"))
            return assetLibraryObject["meshes"];
    
        return {};
    }

    std::vector<CubemapAsset> Project::get_cubemaps() const
    {
        if(assetLibraryObject.contains("cubemaps"))
            return assetLibraryObject["cubemaps"];
    
        return {};
    }

    std::vector<FontAsset> Project::get_fonts() const
    {
        if(assetLibraryObject.contains("fonts"))
            return assetLibraryObject["fonts"];
    
        return {};
    }
}

namespace tur
{
    void Project::read_project()
    {
        std::filesystem::path mainFilepath = get_project_filepath(folderPath.filename().string() + Project::ProjectFileExtension);

        if(!std::filesystem::exists(mainFilepath))
            return create_default_project();

        // Main project:
        nlohmann::json projectObject = read_json(mainFilepath);
        name = projectObject.at("name");

        // Assets:
        std::filesystem::path assetLibraryFilepath = get_project_filepath(std::string("asset_library.asses"));
        if(!std::filesystem::exists(assetLibraryFilepath))
            assetLibraryObject = {};

        assetLibraryObject = deserialize_asset_library(assetLibraryFilepath);

        // Scenes:
        if(projectObject.contains("scenes") && projectObject.at("scenes").is_array())
        {
            for(const auto& object : projectObject["scenes"])
            {
                if(object.is_string())
                    mSceneFilepaths.push_back(object);
            }
        }
    }

    void Project::create_default_project()
    {
        nlohmann::json project;
        project["name"] = name;
        project["scenes"] = nlohmann::json::array();

        write_json(mainFilepath, project);
    }
}
