#pragma once
#include <filesystem>
#include <nlohmann/json.hpp>

#include "graphics/components.hpp"
#include "project/project.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"
#include "utils/json/json_file.hpp"
#include "utils/json/json_common.hpp"

namespace tur
{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AudioSourceComponent, filepath);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HierarchyComponent, parent, level);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UUIDComponent, uuid);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NameComponent, name);
    
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Transform, position, rotation, scale);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TransformComponent, transform);
    
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Sprite2DComponent, filepath);

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TileFlags, data);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Tile, position, layer, flags);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TilemapChunk, chunks);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Tilemap2DComponent, worldData, tilesPerChunk, tilePixelSize, filepath);
    
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Body2DComponent, type, isBullet);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(RectCollider2D, width, height, density, friction, restitution, rollingResistance);
		
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ScriptComponent, filepath);
    
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Camera, position, target);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CameraComponent, mainCamera, camera);
    
    NLOHMANN_MARK_AS_NON_SERIALIZABLE(CullingComponent);

    class JsonOutputArchive
    {
    public:
        JsonOutputArchive()
        {
            mObject = nlohmann::json::array();
        }
        
    public:
        void operator()(entt::entity entity)
        {
            mCurrent.push_back(static_cast<u32>(entity));
        }

        void operator()(std::underlying_type_t<entt::entity> size)
        {
            if (!mCurrent.empty())
                mObject.push_back(mCurrent);
        
            mCurrent = nlohmann::json::array();
            mCurrent.push_back(size);
        }

        template<typename T>
        void operator()(const T& component)
        {
            nlohmann::json componentObject = component;
            mCurrent.push_back(componentObject);
        }

    public:
        void save(const std::filesystem::path& filepath)
        {
            if (!mCurrent.empty())
                mObject.push_back(mCurrent);

            json_write_file(filepath, mObject);
        }

    private:
        nlohmann::json mObject, mCurrent;
    };

    class JSONInputArchive 
    {
    public:
        JSONInputArchive(const nlohmann::json& object)
        {
            mObject = object;
        };

    public:
        void operator()(std::underlying_type_t<entt::entity>& size)
        {
            fetch_next_node();
            
            int currentSize = mCurrent[0].get<int>();
            ++mCurrentIndex;

            size = (std::underlying_type_t<entt::entity>)currentSize;
        }

        void operator()(entt::entity& entity)
        {
            entity = entt::entity(mCurrent[mCurrentIndex].get<uint32_t>());
            ++mCurrentIndex;
        }

        template<typename T>
        void operator()(T& component)
        {
            nlohmann::json componentData = mCurrent[mCurrentIndex];
            component = componentData.get<T>();
            ++mCurrentIndex;
        }

    public:
        void fetch_next_node()
        {
            ++mObjectIndex;

            if (mObjectIndex >= mObject.size())
            {
                TUR_LOG_CRITICAL("Failed to parse json as entt::archive. Calculated index is greater than storage capacity");
                return;
            }

            mCurrent = mObject[mObjectIndex];
            mCurrentIndex = 0;
        }

    private:
        nlohmann::json mObject;
        nlohmann::json mCurrent;

        int mObjectIndex = -1;
        int mCurrentIndex = 0;
    };

    inline void serialize_scene(NON_OWNING Scene* scene, const std::filesystem::path& filepath)
    {
        JsonOutputArchive archive;
        entt::snapshot{scene->get_registry()}
            .get<entt::entity>(archive)
            .get<AudioSourceComponent>(archive)
            .get<CameraComponent>(archive)
            .get<HierarchyComponent>(archive)
            .get<UUIDComponent>(archive)
            .get<NameComponent>(archive)
            .get<TransformComponent>(archive)
            .get<Sprite2DComponent>(archive)
            .get<Tilemap2DComponent>(archive)
            .get<Body2DComponent>(archive)
            .get<RectCollider2D>(archive)
            .get<ScriptComponent>(archive);

        archive.save(filepath);
    }

    inline void deserialize_scene(NON_OWNING Scene* scene, const Project& project, const std::filesystem::path& sceneFilename)
    {
        JSONInputArchive archive(json_parse_file(project.get_project_filepath(sceneFilename)));
        
        entt::snapshot_loader{scene->get_registry()}
            .get<entt::entity>(archive)
            .get<AudioSourceComponent>(archive)
            .get<CameraComponent>(archive)
            .get<HierarchyComponent>(archive)
            .get<UUIDComponent>(archive)
            .get<NameComponent>(archive)
            .get<TransformComponent>(archive)
            .get<Sprite2DComponent>(archive)
            .get<Tilemap2DComponent>(archive)
            .get<Body2DComponent>(archive)
            .get<RectCollider2D>(archive)
            .get<ScriptComponent>(archive)
            .orphans();
    }
}