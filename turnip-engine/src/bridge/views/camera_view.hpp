#pragma once
#include "event/asset_events/scene_deserialization.hpp"
#include "event/scene_events/camera_switched_event.hpp"
#include "graphics/camera.hpp"
#include "graphics/components.hpp"
#include "memory/memory.hpp"
#include "scene/entity.hpp"
#include "event/events.hpp"
#include "utils/aabb.hpp"
#include "utils/extent.hpp"
#include "view/view.hpp"

namespace tur
{
	class CameraView : public View
	{	
    public:
        void on_update(const Time&) override
		{
            rCurrentCamera->update_view();
            cull_scene_orthographic();
		}

        void on_event(Event& event) override
		{
			Subscriber subscriber(event);

            subscriber.subscribe<SceneSwitchedEvent>([this](const SceneSwitchedEvent& event) -> bool {
				rScene = event.currentScene;
                fetch_main_camera(event.currentScene);
				return false;
			});
        }

    private:
        void fetch_main_camera(Scene* scene)
        {
            auto view = scene->get_registry().view<CameraComponent>();
            for(auto [entity, camera] : view.each())
            {
                if(camera.mainCamera)
                    rCurrentCamera = &camera.camera;
            }

            CameraSwitchedEvent switchEvent(rCurrentCamera);
            callback(switchEvent);
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
        NON_OWNING Scene* rScene = nullptr;
	};
}