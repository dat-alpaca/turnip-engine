#pragma once
#include "event/window_events/window_framebuffer_event.hpp"
#include "graphics/components.hpp"
#include "graphics/renderer/imm_quad_renderer.hpp"
#include "logging/logging.hpp"
#include "rhi/rhi.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"
#include "utils/color.hpp"

namespace tur
{
	// TODO: config renderers
	class ImmQuadSystem
	{
	public:
		void initialize(NON_OWNING RenderInterface* rhi, NON_OWNING Scene* scene, NON_OWNING Camera* camera)
		{
			mQuadRenderer.initialize(rhi);
			mQuadRenderer.set_clear_color(color::Blue, ClearFlags::COLOR);
			mQuadRenderer.set_camera(camera);
			set_scene(scene);

			setup_registry_events();
		}
		void set_scene(Scene* scene) { rScene = scene; }
		void set_camera(Camera* camera) { mQuadRenderer.set_camera(camera); }

	public:
		void render() { mQuadRenderer.render(); }

		void on_event(Event& event)
		{
			Subscriber subscriber(event);
			subscriber.subscribe<WindowFramebufferEvent>(
				[&](const WindowFramebufferEvent& resizeEvent) -> bool
				{
					mQuadRenderer.set_viewport({0.f, 0.f, (f32)resizeEvent.width, (f32)resizeEvent.height});
					mQuadRenderer.set_scissor({0, 0, resizeEvent.width, resizeEvent.height});

					return false;
				}
			);
		}

		void on_update()
		{
			auto view = rScene->get_registry().view<TransformComponent, Sprite2DComponent>();
			for (auto [e, transform, sprite] : view.each())
			{
				if (mHandleCache.find(e) == mHandleCache.end())
				{
					mHandleCache[e] = mQuadRenderer.add_quad(transform.worldTransform, sprite.textureHandle);
					break;
				}

				auto& quad = mQuadRenderer.get_quad(mHandleCache[e]);
				quad.transform = transform.worldTransform;
				quad.textureHandle = sprite.textureHandle;
			}
		}

	private:
		void setup_registry_events()
		{
			auto& registry = rScene->get_registry();
			registry.on_construct<Sprite2DComponent>().connect<&ImmQuadSystem::on_imm_component_added>(this);
		}

		void on_imm_component_added(entt::registry& registry, entt::entity entity)
		{
			Entity sceneEntity { entity, rScene };
			sceneEntity.add_component<CullingComponent>();
		}

	public:
		ImmQuadRenderer& renderer() { return mQuadRenderer; }

	private:
		NON_OWNING Scene* rScene = nullptr;
		ImmQuadRenderer mQuadRenderer;
		std::unordered_map<entt::entity, ImmQuadRenderer::quad_handle> mHandleCache;
	};
}