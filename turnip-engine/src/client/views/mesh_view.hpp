#pragma once
#include "view/view.hpp"
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
	class MeshView : public View
	{
	public:
		MeshView(NON_OWNING RenderInterface* rhi)
		{
			mRenderer.initialize(rhi);
			mRenderer.set_clear_color(color::Blue, ClearFlags::COLOR);
		}

	public:
		void render_deferred()
		{ 
			mRenderer.render(); 
		}
	
	public:
		void on_event(Event& event) override
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

			subscriber.subscribe<SceneCreatedEvent>([this](const SceneCreatedEvent& event) -> bool {
				auto& registry = event.scene->get_registry();
				registry.on_construct<MeshComponent>().connect<&MeshView::on_mesh_component_added>(this);
				return false;
			});

			subscriber.subscribe<SceneSwitchedEvent>([this](const SceneSwitchedEvent& event) -> bool {
				rScene = event.currentScene;
				return false;
			});
			
			subscriber.subscribe<CameraSwitchedEvent>([this](const CameraSwitchedEvent& event) -> bool {
				mRenderer.set_camera(event.camera);
				return false;
			});
		}

		void on_update(const Time&) override
		{
            auto view = rScene->get_registry().view<TransformComponent, MeshComponent, const CullingComponent>();
			for (auto [e, transform, mesh, culling] : view.each())
			{
				if(!mesh.material.is_valid())
					continue;

				if(mesh.vbo == invalid_handle || mesh.ebo == invalid_handle)
					continue;

				if(mMeshCache.find(e) == mMeshCache.end())
				{
					MeshRenderer::InternalMesh internal;
					internal.ebo = mesh.ebo;
					internal.vbo = mesh.vbo;
					internal.indexCount = mesh.indexCount;
					internal.isVisible = culling.visible;
					internal.indexType = mesh.indexType;

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
		void on_mesh_component_added(entt::registry& registry, entt::entity entity)
		{
			if(!registry.all_of<TransformComponent>(entity))
				registry.emplace<TransformComponent>(entity);
			
			if(!registry.all_of<CullingComponent>(entity))
				registry.emplace<CullingComponent>(entity);
        }

	public:
		MeshRenderer& renderer() { return mRenderer; }

	private:
        NON_OWNING RenderInterface* rRHI = nullptr;
		NON_OWNING Scene* rScene = nullptr;

		std::unordered_map<entt::entity, handle_type> mMeshCache;
		MeshRenderer mRenderer;
	};
}