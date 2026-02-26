#pragma once
#include <json.hpp>
#include <type_traits>
#include <unordered_map>

#include "defines.hpp"
#include "components.hpp"
#include "entt/entity/entity.hpp"
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
            mCurrentComponentName = &mEntityMap[entity].back().first;
            mCurrentValue = &mEntityMap[entity].back().second;
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
        requires NamedComponent<T>
        void operator()(const T& component)
        {
            *mCurrentComponentName = T::get_component_name();
            *mCurrentValue = component;
        }

    public:
        void save(const std::filesystem::path& filepath)
        {
            nlohmann::json output;
            output["entities"] = mEntityList;

            nlohmann::json entityList = nlohmann::json::object();
            for(auto& [entity, componentPair] : mEntityMap)
            {
                std::string key = std::to_string(static_cast<u32>(entity));
                
                nlohmann::json entityObject = nlohmann::json::object();
                for(const auto& [componentName, component] : componentPair)
                    entityObject[componentName] = component;

                entityList[key] = entityObject;
            }

            output["components"] = entityList;
            write_json(filepath, output);
        }

    private:
        u32 mEntityCount = invalid_handle;
        std::vector<entt::entity> mEntityList;

        nlohmann::json* mCurrentValue = nullptr;
        std::string* mCurrentComponentName = nullptr;

        std::unordered_map<
            entt::entity, 
            std::vector<std::pair<std::string, nlohmann::json>>
        > mEntityMap;
    };

    class JSONInputArchive
    {
    public:
        JSONInputArchive(const nlohmann::json& object)
        {
            mEntities = object["entities"].get<std::vector<entt::entity>>();
            mComponents = object["components"];
        }

        void operator()(entt::entity& entity)
        {
            entity = static_cast<entt::entity>(mEntities[mEntityIndex++]);

            std::string key = std::to_string(static_cast<u32>(entity));

            auto it = mComponents.find(key);
            mCurrentComponents = (it != mComponents.end()) ? &(*it) : nullptr;
        }

        void operator()(std::underlying_type_t<entt::entity>& size)
        {
            size = static_cast<std::underlying_type_t<entt::entity>>(mEntities.size());
            mEntityIndex = 0;
        }

        template<typename T>
        requires NamedComponent<T>
        void operator()(T& component)
        {
            if(!mCurrentComponents)
                return;

            auto key = T::get_component_name();

            auto it = mCurrentComponents->find(key);
            if(it == mCurrentComponents->end())
                return;

            component = it->template get<T>();
        }

    private:
        nlohmann::json* mCurrentComponents = nullptr;
        std::vector<entt::entity> mEntities;
        nlohmann::json mComponents;
        u32 mEntityIndex = 0;
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