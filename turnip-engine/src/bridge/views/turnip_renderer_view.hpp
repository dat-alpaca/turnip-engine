#pragma once
#include "graphics/renderer/turnip/turnip_renderer.hpp"

#include "event/events.hpp"
#include "scene/components.hpp"
#include "scene/components/graphics.hpp"
#include "scene/scene.hpp"
#include "utils/color.hpp"
#include "view/view.hpp"

namespace tur
{
	class TurnipRendererView : public View
	{
	public:
		TurnipRendererView(NON_OWNING RenderInterface* rhi)
		{
			mRenderer.initialize(rhi);
		}

	public:
		void render_deferred()
		{ 
			mRenderer.render(); 
		}

		void dispatch()
		{
			mRenderer.dispatch();
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
			if(!rScene)
				return;

			auto view = rScene->get_registry().view<TransformComponent, MeshComponent>();
			for (auto [entity, transform, mesh] : view.each())
			{
				/* Blank */
			}
		}

	public:
		TurnipRenderer& renderer() { return mRenderer; }

	private:
        NON_OWNING Scene* rScene = nullptr;
		TurnipRenderer mRenderer;
	};
}