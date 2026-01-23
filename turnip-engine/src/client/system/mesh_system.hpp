#pragma once
#include "client/system/system.hpp"
#include "defines.hpp"
#include "graphics/components.hpp"
#include "graphics/renderer/mesh_renderer.hpp"
#include "scene/common_components.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"
#include "utils/color.hpp"
#include <unordered_map>

namespace tur
{
	class MeshSystem : public System
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
			for (auto [e, transform, mesh, culling] : view.each())
			{
				if(mMeshCache.find(e) == mMeshCache.end())
				{
					MeshRenderer::InternalMesh internal;
					internal.ebo = mesh.ebo;
					internal.vbo = mesh.vbo;
					internal.indexCount = mesh.indexCount;
					internal.isVisible = culling.visible;

					internal.material = mesh.material;

					mMeshCache[e] = mRenderer.add_mesh(internal);
					continue;
				}

				auto& rendererMesh = mRenderer.get_mesh(mMeshCache[e]);

				rendererMesh.ebo = mesh.ebo;
				rendererMesh.vbo = mesh.vbo;
				rendererMesh.indexCount = mesh.indexCount;
				rendererMesh.material = mesh.material;
				rendererMesh.isVisible = culling.visible;
				rendererMesh.transform = transform.worldTransform.transform();
			}
		}

	private:
		void setup_registry_events()
		{
			auto& registry = rScene->get_registry();
			registry.on_construct<MeshComponent>().connect<&MeshSystem::on_mesh_component_added>(this);
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
		MeshRenderer& renderer() { return mRenderer; }

	private:
        NON_OWNING RenderInterface* rRHI = nullptr;
        std::unordered_map<entt::entity, handle_type> mMeshCache;
		MeshRenderer mRenderer;
	};
}