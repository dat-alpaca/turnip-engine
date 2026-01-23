#pragma once
#include <nlohmann/json.hpp>
#include <filesystem>
#include <vector>
#include <string>

#include "assets/cubemap/cubemap_asset.hpp"
#include "assets/mesh/mesh_asset.hpp"
#include "scene/scene.hpp"
#include "assets/audio/audio_asset.hpp"
#include "assets/texture/texture_asset.hpp"

namespace tur
{
    class AssetLibrary;

    struct Project
    {
    public:
        Project(const std::filesystem::path& folderPath);
        Project() = default;

    public:
        void initialize(const std::filesystem::path& folderPath);
    
    public:
        void save();
        void save_asset_library(NON_OWNING AssetLibrary* library);
        void add_scene(NON_OWNING Scene* scene, const std::filesystem::path& filepath);
    
    public:
        std::vector<TextureAsset> get_textures() const;
        std::vector<AudioAsset> get_audios() const;
        std::vector<MeshAsset> get_meshes() const;
        std::vector<CubemapAsset> get_cubemaps() const;
    
        inline std::filesystem::path get_project_filepath(const std::filesystem::path& filepath) const
        {
            return folderPath / filepath;
        }
        inline std::vector<std::filesystem::path> get_scene_filepaths() const
        {
            return mSceneFilepaths;
        }

    private:
        void read_project();

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