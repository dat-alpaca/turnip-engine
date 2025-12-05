#pragma once
#include <filesystem>
#include <nlohmann/json.hpp>

#include "entt/entity/fwd.hpp"
#include "logging/logging.hpp"
#include "physics/physics_components.hpp"
#include "scene/common_components.hpp"
#include "scene/scene.hpp"
#include "scene/entity.hpp"
#include "scene/components.hpp"

#include "utils/json/json_file.hpp"

namespace tur
{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AudioSourceComponent, assetHandle);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HierarchyComponent, parent, level);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UUIDComponent, uuid);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NameComponent, name);
    
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Transform, position, rotation, scale);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TransformComponent, transform);
    
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Sprite2DComponent, textureHandle, assetHandle);
    
    // TODO: NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Body2DComponent, bodyID);

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ScriptComponent, filepath, instance);

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
            uint32_t entityID = mCurrent[mCurrentIndex].get<uint32_t>();
            entity = entt::entity(entityID);
            ++mCurrentIndex;
        }

        template<typename T>
        void operator()(T& component)
        {
            nlohmann::json componentData = mCurrent[mCurrentIndex * 2];

            auto componentFetched = componentData.get<T>();
            component = componentFetched;

            uint32_t entityID = mCurrent[mCurrentIndex * 2 - 1];
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
        // TODO: serialize body2d
        JsonOutputArchive archive;
        entt::snapshot{scene->get_registry()}
            .get<entt::entity>(archive)
            .get<AudioSourceComponent>(archive)
            .get<HierarchyComponent>(archive)
            .get<UUIDComponent>(archive)
            .get<NameComponent>(archive)
            .get<TransformComponent>(archive)
            .get<Sprite2DComponent>(archive)
            // .get<Body2DComponent>(archive)
            .get<ScriptComponent>(archive);

        archive.save(filepath);
    }

    inline void deserialize_scene(NON_OWNING Scene* scene, const std::filesystem::path& filepath)
    {
        // TODO: serialize body2d

        JSONInputArchive archive(json_parse_file(filepath));
        entt::snapshot_loader{scene->get_registry()}
            .get<entt::entity>(archive)
            .get<AudioSourceComponent>(archive)
            .get<HierarchyComponent>(archive)
            .get<UUIDComponent>(archive)
            .get<NameComponent>(archive)
            .get<TransformComponent>(archive)
            .get<Sprite2DComponent>(archive)
            // .get<Body2DComponent>(archive)
            .get<ScriptComponent>(archive)
            .orphans();
    }
}