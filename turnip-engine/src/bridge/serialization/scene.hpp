#pragma once
#include <json.hpp>
#include <string>
#include <type_traits>
#include <unordered_map>

#include "defines.hpp"
#include "components.hpp"
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include "logging/logging.hpp"
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

    template<typename T>
    requires NamedComponent<T>
    inline void deserialize(NON_OWNING Scene* scene, const nlohmann::json& object)
    {
        auto& registry = scene->get_registry();
        std::string key = T::get_component_name();
        
        for(auto& [entityName, components] : object.items())
        {
            entt::entity entityID = static_cast<entt::entity>(std::stoul(entityName));

            for(auto& [componentName, component] : components.items())
            {
                if(key != componentName)
                    continue;

                registry.emplace_or_replace<T>(entityID, components[key].get<T>());
            }
        }
    }

    inline void deserialize_scene(NON_OWNING Scene* scene, const Project& project, const std::filesystem::path& sceneFilename)
    {
        auto& registry = scene->get_registry();
        nlohmann::json fileObject = read_json(project.get_project_filepath(sceneFilename));

        for(auto& [entityName, _] : fileObject.items())
            entt::entity entity = registry.create(static_cast<entt::entity>(std::stoul(entityName)));


        deserialize<UUIDComponent>(scene, fileObject);
        deserialize<NameComponent>(scene, fileObject);
        deserialize<HierarchyComponent>(scene, fileObject);
        deserialize<TransformComponent>(scene, fileObject);
        deserialize<Body2DComponent>(scene, fileObject);
        deserialize<RectCollider2D>(scene, fileObject);
        deserialize<AudioSourceComponent>(scene, fileObject);
        deserialize<CameraComponent>(scene, fileObject);
        deserialize<Sprite2DComponent>(scene, fileObject);
        deserialize<CubemapComponent>(scene, fileObject);
        deserialize<MeshComponent>(scene, fileObject);
        deserialize<Tilemap2DComponent>(scene, fileObject);
        deserialize<TextComponent>(scene, fileObject);
        deserialize<ScriptComponent>(scene, fileObject);
    }
}