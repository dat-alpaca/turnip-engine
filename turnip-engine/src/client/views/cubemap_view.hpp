#pragma once
#include "defines.hpp"
#include "entt/entity/fwd.hpp"
#include "event/window_events/window_framebuffer_event.hpp"
#include "graphics/components.hpp"
#include "graphics/renderer/cubemap_renderer.hpp"
#include "memory/memory.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"
#include "utils/color.hpp"
#include "view/view.hpp"

namespace tur
{
	class CubemapView : public View
	{
	public:
		CubemapView(NON_OWNING RenderInterface* rhi)
		{
			mCubemapRenderer.initialize(rhi);
			mCubemapRenderer.set_clear_color(color::Blue, ClearFlags::COLOR);
		}
	
	public:
		void render_deferred()
		{ 
			mCubemapRenderer.render(); 
		}

	public:
		void on_event(Event& event) override
		{
			Subscriber subscriber(event);
			subscriber.subscribe<SceneSwitchedEvent>([this](const SceneSwitchedEvent& event) -> bool {
				rScene = event.currentScene;
				return false;
			});

			subscriber.subscribe<CameraSwitchedEvent>([this](const CameraSwitchedEvent& event) -> bool {
				mCubemapRenderer.set_camera(event.camera);
				return false;
			});

			subscriber.subscribe<WindowFramebufferEvent>(
				[&](const WindowFramebufferEvent& resizeEvent) -> bool
				{
					mCubemapRenderer.set_viewport({0.f, 0.f, (f32)resizeEvent.width, (f32)resizeEvent.height});
					mCubemapRenderer.set_scissor({0, 0, resizeEvent.width, resizeEvent.height});

					return false;
				}
			);
		}

		void on_update(const Time&) override
		{
			auto view = rScene->get_registry().view<CubemapComponent>();
			for (auto [e, cubemap] : view.each())
            {
				if(cubemap.textureHandle == invalid_handle)
					continue;
				
				mCubemapRenderer.set_cubemap_details(cubemap.textureHandle);
                cubemapSet = true;
                return;
            }
		}

	public:
		CubemapRenderer& renderer() { return mCubemapRenderer; }

	private:
		CubemapRenderer mCubemapRenderer;
        bool cubemapSet = false;
	
	private:
		NON_OWNING Scene* rScene = nullptr;
	};
}