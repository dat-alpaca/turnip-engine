#pragma once
#include "graphics/renderer/imm_quad_renderer.hpp"
#include "rhi/rhi.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"

namespace tur
{
	class ImmQuadSystem
	{
	public:
		void initialize(NON_OWNING RenderInterface* rhi, NON_OWNING Scene* scene, NON_OWNING Camera* camera)
		{
			mQuadRenderer.initialize(rhi);
			mQuadRenderer.set_clear_color({}, ClearFlags::COLOR);
			mQuadRenderer.set_camera(camera);
			set_scene(scene);
		}
		void set_scene(Scene* scene) { rScene = scene; }

		void render() { mQuadRenderer.render(); }

		void on_event(Event& event)
		{
			Subscriber subscriber(event);
			subscriber.subscribe<WindowResizeEvent>(
				[&](const WindowResizeEvent& resizeEvent) -> bool
				{
					mQuadRenderer.set_viewport({0.f, 0.f, (f32)resizeEvent.width, (f32)resizeEvent.height});
					mQuadRenderer.set_scissor({0, 0, resizeEvent.width, resizeEvent.height});

					return false;
				}
			);
		}

		void update()
		{
			auto view = rScene->get_registry().view<TransformComponent, Sprite2DComponent>();
			for (auto [e, transform, sprite] : view.each())
			{
				if (mHandleCache.find(e) == mHandleCache.end())
				{
					mHandleCache[e] = mQuadRenderer.add_quad(transform.transform, sprite.textureHandle);
					break;
				}

				auto& quad = mQuadRenderer.get_quad(mHandleCache[e]);
				quad.transform = transform.transform;
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