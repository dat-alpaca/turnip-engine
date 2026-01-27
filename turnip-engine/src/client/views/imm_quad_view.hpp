#pragma once
#include "event/scene_events/camera_switched_event.hpp"
#include "memory/memory.hpp"
#include "view/view.hpp"
#include "event/window_events/window_framebuffer_event.hpp"
#include "graphics/components.hpp"
#include "graphics/renderer/imm_quad_renderer.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"
#include "utils/color.hpp"

namespace tur
{
	class ImmQuadView : public View
	{
	public:
		ImmQuadView(NON_OWNING RenderInterface* rhi)
		{
			mQuadRenderer.initialize(rhi);
			mQuadRenderer.set_clear_color(color::Blue, ClearFlags::COLOR);
		}

	public:
		void render_deferred()
		{ 
			mQuadRenderer.render(); 
		}
	
	public:
		void on_event(Event& event) override
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

			subscriber.subscribe<SceneCreatedEvent>([this](const SceneCreatedEvent& event) -> bool {
				auto& registry = event.scene->get_registry();
				registry.on_construct<Sprite2DComponent>().connect<&ImmQuadView::on_imm_component_added>(this);
				return false;
			});

			subscriber.subscribe<SceneSwitchedEvent>([this](const SceneSwitchedEvent& event) -> bool {
				rScene = event.currentScene;
				return false;
			});

			subscriber.subscribe<CameraSwitchedEvent>([this](const CameraSwitchedEvent& event) -> bool {
				mQuadRenderer.set_camera(event.camera);
				return false;
			});			
		}

		void on_update(const Time&) override
		{
			auto view = rScene->get_registry().view<TransformComponent, Sprite2DComponent, const CullingComponent>();
			for (auto [e, transform, sprite, culling] : view.each())
			{
				if (mHandleCache.find(e) == mHandleCache.end())
				{
					mHandleCache[e] = mQuadRenderer.add_quad(transform.worldTransform, sprite.textureHandle);
					break;
				}

				auto& quad = mQuadRenderer.get_quad(mHandleCache[e]);
				quad.transform = transform.worldTransform;
				quad.textureHandle = sprite.textureHandle;

				quad.drawQuad = culling.visible;
			}
		}

	private:
		void on_imm_component_added(entt::registry& registry, entt::entity entity)
		{
			registry.emplace<CullingComponent>(entity);
		}

	public:
		ImmQuadRenderer& renderer() { return mQuadRenderer; }

	private:
		ImmQuadRenderer mQuadRenderer;
		std::unordered_map<entt::entity, ImmQuadRenderer::quad_handle> mHandleCache;
		NON_OWNING Scene* rScene = nullptr;
	};
}