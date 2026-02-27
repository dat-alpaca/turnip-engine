#pragma once
#include <json.hpp>
#include <string>

#include "defines.hpp"
#include "components.hpp"
#include "entt/entity/fwd.hpp"
#include "utils/json/json_file.hpp"
#include "bridge/project/project.hpp"

namespace tur
{
    template<typename T>
    requires NamedComponent<T>
    inline void serialize(entt::registry& registry, entt::entity entity, nlohmann::json& object)
    {
        object[T::get_component_name()] = registry.get<T>(entity);
    }

    inline void serialize_scene(NON_OWNING Scene* scene, const std::filesystem::path& filepath)
    {
        // TODO: test

        auto& registry = scene->get_registry();
        nlohmann::json fileObject = nlohmann::json::object();

        for(auto entity : scene->get_registry().view<entt::entity>())
        {
            auto entityKey = std::to_string(static_cast<u32>(entity));
            fileObject[entityKey] = nlohmann::json::object();
            auto& entityObject = fileObject[entityKey];

            serialize<UUIDComponent>(registry, entity, entityObject);
            serialize<NameComponent>(registry, entity, entityObject);
            serialize<HierarchyComponent>(registry, entity, entityObject);
            serialize<TransformComponent>(registry, entity, entityObject);
            serialize<Body2DComponent>(registry, entity, entityObject);
            serialize<RectCollider2D>(registry, entity, entityObject);
            serialize<AudioSourceComponent>(registry, entity, entityObject);
            serialize<CameraComponent>(registry, entity, entityObject);
            serialize<Sprite2DComponent>(registry, entity, entityObject);
            serialize<CubemapComponent>(registry, entity, entityObject);
            serialize<MeshComponent>(registry, entity, entityObject);
            serialize<Tilemap2DComponent>(registry, entity, entityObject);
            serialize<TextComponent>(registry, entity, entityObject);
            serialize<ScriptComponent>(registry, entity, entityObject);
        }
    }

    template<typename T>
    requires NamedComponent<T>
    inline void deserialize(entt::registry& registry, const nlohmann::json& object)
    {
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

        deserialize<UUIDComponent>(registry, fileObject);
        deserialize<NameComponent>(registry, fileObject);
        deserialize<HierarchyComponent>(registry, fileObject);
        deserialize<TransformComponent>(registry, fileObject);
        deserialize<Body2DComponent>(registry, fileObject);
        deserialize<RectCollider2D>(registry, fileObject);
        deserialize<AudioSourceComponent>(registry, fileObject);
        deserialize<CameraComponent>(registry, fileObject);
        deserialize<Sprite2DComponent>(registry, fileObject);
        deserialize<CubemapComponent>(registry, fileObject);
        deserialize<MeshComponent>(registry, fileObject);
        deserialize<Tilemap2DComponent>(registry, fileObject);
        deserialize<TextComponent>(registry, fileObject);
        deserialize<ScriptComponent>(registry, fileObject);
    }
}