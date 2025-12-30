#pragma once
#include "graphics/camera.hpp"
#include "graphics/components.hpp"
#include "scene/scene.hpp"
#include "scene/entity.hpp"

namespace tur
{
	class CameraSystem
	{
	public:
		void initialize(NON_OWNING Scene* scene)
		{
			set_scene(scene);
            setup_registry_events();
		}
		void set_scene(Scene* scene) { rScene = scene; }
	
    public:
        void update()
		{
            rCurrentCamera->update_view();
		}

    private:
        void setup_registry_events()
        {
            auto& registry = rScene->get_registry();
			registry.on_construct<CameraComponent>().connect<&CameraSystem::on_camera_added_or_updated>(this);
            registry.on_update<CameraComponent>().connect<&CameraSystem::on_camera_added_or_updated>(this);
        }

        void on_camera_added_or_updated(entt::registry& registry, entt::entity entity)
        {
            Entity sceneEntity { entity, rScene };
			auto& camera = sceneEntity.get_component<CameraComponent>();

            if(camera.mainCamera)
                rCurrentCamera = &camera.camera;
        }

    public:
        NON_OWNING Camera* get_main_camera() const { return rCurrentCamera; }

	private:
		NON_OWNING Scene* rScene = nullptr;
        NON_OWNING Camera* rCurrentCamera = nullptr;
	};
}