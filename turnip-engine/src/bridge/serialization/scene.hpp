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

    class JSONInputArchive
    {
    public:
        JSONInputArchive(const nlohmann::json& object)
        {
            mEntityList = object["entities"].get<std::vector<entt::entity>>();
            mEntityCount = static_cast<u32>(mEntityList.size());

            const auto& components = object["components"];
            for (auto& [key, value] : components.items())
            {
                entt::entity entity = static_cast<entt::entity>(std::stoul(key));
                mEntityMap[entity] = value.get<std::vector<nlohmann::json>>();
            }
        }

        void operator()(std::underlying_type_t<entt::entity>& size)
        {
            size = mEntityCount;
        }

        void operator()(entt::entity& entity)
        {
            if (mEntityIndex < mEntityList.size())
            {
                entity = mEntityList[mEntityIndex++];
                mCurrentEntity = entity;
                mComponentIndex = 0;
                return;
            }

            mCurrentEntity = entity;
            mComponentIndex = 0;
        }

        template<typename T>
        void operator()(T& component)
        {
            auto& componentArray = mEntityMap[mCurrentEntity];

            if (mComponentIndex >= componentArray.size())
                return;

            component = componentArray[mComponentIndex++].get<T>();
        }

    private:
        std::vector<entt::entity> mEntityList;
        std::unordered_map<entt::entity, std::vector<nlohmann::json>> mEntityMap;
    
        u32 mEntityCount = 0;
        u32 mEntityIndex = 0;
        u32 mComponentIndex = 0;
        entt::entity mCurrentEntity = entt::null;
    };

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
        JSONInputArchive archive(read_json(project.get_project_filepath(sceneFilename)));
        
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
            .orphans();
    }
}