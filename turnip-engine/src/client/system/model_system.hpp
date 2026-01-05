#pragma once
#include "client/system/system.hpp"
#include "defines.hpp"
#include "graphics/components.hpp"
#include "graphics/renderer/model_renderer.hpp"
#include "scene/common_components.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"
#include "utils/color.hpp"
#include <unordered_map>

namespace tur
{
	class ModelSystem : public System
	{
	public:
		void initialize(NON_OWNING RenderInterface* rhi, NON_OWNING Scene* scene, NON_OWNING Camera* camera)
		{
			mRenderer.initialize(rhi);
			mRenderer.set_clear_color(color::Blue, ClearFlags::COLOR);
			mRenderer.set_camera(camera);
			set_scene(scene);

			setup_registry_events();
		}
		void set_camera(Camera* camera) { mRenderer.set_camera(camera); }

	public:
		void render() { mRenderer.render(); }

		void on_event(Event& event)
		{
			Subscriber subscriber(event);
			subscriber.subscribe<WindowFramebufferEvent>(
				[&](const WindowFramebufferEvent& resizeEvent) -> bool
				{
					mRenderer.set_viewport({0.f, 0.f, (f32)resizeEvent.width, (f32)resizeEvent.height});
					mRenderer.set_scissor({0, 0, resizeEvent.width, resizeEvent.height});

					return false;
				}
			);
		}

		void on_update()
		{
            auto view = rScene->get_registry().view<TransformComponent, MeshComponent, const CullingComponent>();
			for (auto [e, transform, model, culling] : view.each())
			{
				if(mModelCache.find(e) == mModelCache.end())
				{
					ModelRenderer::InternalModel internal;
					internal.ebo = model.ebo;
					internal.vbo = model.vbo;
					internal.indexCount = model.indexCount;
					internal.isVisible = culling.visible;

					internal.albedo = model.albedo;

					mModelCache[e] = mRenderer.add_model(internal);
					continue;
				}

				mRenderer.get_model(mModelCache[e]).isVisible = culling.visible;
				mRenderer.get_model(mModelCache[e]).transform = transform.worldTransform.transform();
			}
		}

	private:
		void setup_registry_events()
		{
			auto& registry = rScene->get_registry();
			registry.on_construct<MeshComponent>().connect<&ModelSystem::on_mesh_component_added>(this);
		}

		void on_mesh_component_added(entt::registry& registry, entt::entity entity)
		{
			Entity sceneEntity { entity, rScene };

			if(!sceneEntity.has_component<TransformComponent>())
				sceneEntity.add_component<TransformComponent>();
			
			if(!sceneEntity.has_component<CullingComponent>())	
				sceneEntity.add_component<CullingComponent>();
        }

	public:
		ModelRenderer& renderer() { return mRenderer; }

	private:
        NON_OWNING RenderInterface* rRHI = nullptr;
        std::unordered_map<entt::entity, handle_type> mModelCache;
		ModelRenderer mRenderer;
	};
}