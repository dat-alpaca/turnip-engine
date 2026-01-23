#pragma once
#include "client/system/system.hpp"
#include "entt/entity/fwd.hpp"
#include "event/window_events/window_framebuffer_event.hpp"
#include "graphics/components.hpp"
#include "graphics/renderer/cubemap_renderer.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"
#include "utils/color.hpp"

namespace tur
{
	class CubemapSystem : public System
	{
	public:
		void initialize(NON_OWNING RenderInterface* rhi, NON_OWNING Scene* scene, NON_OWNING Camera* camera)
		{
			mCubemapRenderer.initialize(rhi);
			mCubemapRenderer.set_clear_color(color::Blue, ClearFlags::COLOR);
			mCubemapRenderer.set_camera(camera);
			set_scene(scene);

			setup_registry_events();
		}
		void set_camera(Camera* camera) { mCubemapRenderer.set_camera(camera); }

	public:
		void render() { mCubemapRenderer.render(); }

		void on_event(Event& event)
		{
			Subscriber subscriber(event);
			subscriber.subscribe<WindowFramebufferEvent>(
				[&](const WindowFramebufferEvent& resizeEvent) -> bool
				{
					mCubemapRenderer.set_viewport({0.f, 0.f, (f32)resizeEvent.width, (f32)resizeEvent.height});
					mCubemapRenderer.set_scissor({0, 0, resizeEvent.width, resizeEvent.height});

					return false;
				}
			);
		}

		void on_update()
		{
			auto view = rScene->get_registry().view<CubemapComponent>();
			for (auto [e, cubemap] : view.each())
            {
				mCubemapRenderer.set_cubemap_details(cubemap.textureHandle);
                cubemapSet = true;
                return;
            }
		}

	private:
		void setup_registry_events()
		{
            /* Blank */
		}

	public:
		CubemapRenderer& renderer() { return mCubemapRenderer; }

	private:
		CubemapRenderer mCubemapRenderer;
        bool cubemapSet = false;
	};
}