#pragma once
#include "event/window_events/window_framebuffer_event.hpp"
#include "graphics/renderer/imm_quad_renderer.hpp"
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
		}
		void set_scene(Scene* scene) { rScene = scene; }
		void set_camera(Camera* camera) { mQuadRenderer.set_camera(camera); }

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

	public:
		ImmQuadRenderer& renderer() { return mQuadRenderer; }

	private:
		NON_OWNING Scene* rScene = nullptr;
		ImmQuadRenderer mQuadRenderer;
		std::unordered_map<entt::entity, ImmQuadRenderer::quad_handle> mHandleCache;
	};
}