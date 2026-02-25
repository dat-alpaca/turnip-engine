#pragma once
#include <json.hpp>
#include <type_traits>
#include <unordered_map>

#include "defines.hpp"
#include "components.hpp"
#include "utils/json/json_file.hpp"
#include "bridge/project/project.hpp"

namespace tur
{
    class JSONOutputArchive
    {
    public:
        void operator()(entt::entity entity)
        {
            if(mEntityList.size() != mEntityCount)
            {
                mEntityList.push_back(entity);
                return;
            }

            if(mEntityMap.find(entity) == mEntityMap.end())
                mEntityMap[entity] = {};

            mEntityMap[entity].push_back({});
            mCurrentValue = &mEntityMap[entity].back();
        }

        void operator()(std::underlying_type_t<entt::entity> size)
        {
            // Header:
            if(mEntityCount == invalid_handle)
            {
                mEntityCount = size;
                mEntityList.reserve(size);
                return;
            }
        }

        template<typename T>
        void operator()(const T& component)
        {
            *mCurrentValue = component;
        }

    public:
        void save(const std::filesystem::path& filepath)
        {
            nlohmann::json output;
            output["entities"] = mEntityList;

            nlohmann::json entityList;
            for(auto& [entity, components] : mEntityMap)
            {
                std::string key = std::string(std::to_string(static_cast<u32>(entity)));
                entityList[key] = nlohmann::json::array();

                for(const auto& component : components)
                    entityList[key].push_back(component);
            }
            output["components"] = entityList;

            write_json(filepath, output);
        }

    private:
        u32 mEntityCount = invalid_handle;
        std::vector<entt::entity> mEntityList;

        nlohmann::json* mCurrentValue = nullptr;
        std::unordered_map<entt::entity, std::vector<nlohmann::json>> mEntityMap;
    };

    /*
    TODO:
    class JSONInputArchive 
    {
    public:
        JSONInputArchive(const toml::value& object)
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
        toml::value mObject;
        toml::value mCurrent;

        int mObjectIndex = -1;
        int mCurrentIndex = 0;
    };
    */

    inline void serialize_scene(NON_OWNING Scene* scene, const std::filesystem::path& filepath)
    {
        JSONOutputArchive archive;
        entt::snapshot{scene->get_registry()}
            .get<entt::entity>(archive)
            .get<AudioSourceComponent>(archive)
            .get<CameraComponent>(archive)
            .get<HierarchyComponent>(archive)
            .get<UUIDComponent>(archive)
            .get<NameComponent>(archive)
            .get<TransformComponent>(archive)
            .get<Sprite2DComponent>(archive)
            .get<CubemapComponent>(archive)
            .get<MeshComponent>(archive)
            .get<Tilemap2DComponent>(archive)
            .get<TextComponent>(archive)
            .get<Body2DComponent>(archive)
            .get<RectCollider2D>(archive)
            .get<ScriptComponent>(archive);

        archive.save(filepath);
    }

    inline void deserialize_scene(NON_OWNING Scene* scene, const Project& project, const std::filesystem::path& sceneFilename)
    {
        return;

        /*TOMLInputArchive archive(read_toml(project.get_project_filepath(sceneFilename)));
        
        entt::snapshot_loader{scene->get_registry()}
            .get<entt::entity>(archive)
            .get<AudioSourceComponent>(archive)
            .get<CameraComponent>(archive)
            .get<HierarchyComponent>(archive)
            .get<UUIDComponent>(archive)
            .get<NameComponent>(archive)
            .get<TransformComponent>(archive)
            .get<Sprite2DComponent>(archive)
            .get<CubemapComponent>(archive)
            .get<MeshComponent>(archive)
            .get<Tilemap2DComponent>(archive)
            .get<TextComponent>(archive)
            .get<Body2DComponent>(archive)
            .get<RectCollider2D>(archive)
            .get<ScriptComponent>(archive)
            .orphans();*/
    }
}