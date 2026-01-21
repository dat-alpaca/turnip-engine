#pragma once
#include "client/system/system.hpp"
#include "graphics/camera.hpp"
#include "graphics/components.hpp"
#include "scene/entity.hpp"
#include "utils/aabb.hpp"
#include "utils/extent.hpp"

namespace tur
{
	class CameraSystem : public System
	{
	public:
		void initialize(NON_OWNING Scene* scene)
		{
			set_scene(scene);
            setup_registry_events();
		}
	
    public:
        void on_update()
		{
            rCurrentCamera->update_view();

            cull_scene_orthographic();
		}

        void on_scene_switched()
        {
            
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

        void cull_scene_orthographic()
        {
            if(!rCurrentCamera->is_orthographic())
                return;

            auto view = rScene->get_registry().view<TransformComponent, CullingComponent>();
			for (auto entity : view)
			{
				auto& transform = view.get<TransformComponent>(entity).worldTransform;
				auto& culling = view.get<CullingComponent>(entity);
                culling.visible = false;

				const auto& dimensions = rCurrentCamera->dimensions();
				const auto& position = rCurrentCamera->position;

                Rect cameraRect;
                {
                    cameraRect.x = position.x;
                    cameraRect.y = position.y;
                    cameraRect.width = std::abs(dimensions.right - dimensions.left);
                    cameraRect.height = std::abs(dimensions.top - dimensions.bottom);
                }

                Rect objectRect;
                {
                    objectRect.x = transform.position.x - (transform.scale.x * 0.5f);
                    objectRect.y = transform.position.y - (transform.scale.y * 0.5f);
                    objectRect.width = transform.scale.x;
                    objectRect.height = transform.scale.y;
                }

                culling.visible = is_colliding_aabb(cameraRect, objectRect);
			}
        }

    public:
        NON_OWNING Camera* get_main_camera() const { return rCurrentCamera; }

	private:
        NON_OWNING Camera* rCurrentCamera = nullptr;
	};
}