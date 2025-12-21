#pragma once
#include "graphics/components.hpp"
#include "graphics/renderer/tilemap_renderer.hpp"
#include "rhi/rhi.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"
#include "time/time.hpp"

namespace tur
{
	class TilemapSystem
	{
	public:
		void initialize(NON_OWNING RenderInterface* rhi, NON_OWNING Scene* scene, NON_OWNING Camera* camera)
		{
			mTilemapRenderer.initialize(rhi);
			mTilemapRenderer.set_clear_color({}, ClearFlags::COLOR);
			mTilemapRenderer.set_camera(camera);
			set_scene(scene);
		}
		void set_scene(Scene* scene) { rScene = scene; }
    
    public:
		void render() { mTilemapRenderer.render(); }

        void on_event(Event& event)
		{
			Subscriber subscriber(event);
			subscriber.subscribe<WindowResizeEvent>(
				[&](const WindowResizeEvent& resizeEvent) -> bool
				{
					mTilemapRenderer.set_viewport({0.f, 0.f, (f32)resizeEvent.width, (f32)resizeEvent.height});
					mTilemapRenderer.set_scissor({0, 0, resizeEvent.width, resizeEvent.height});

					return false;
				}
			);
		}

		void update(const Time& time)
		{
			auto view = rScene->get_registry().view<Tilemap2DComponent>();
			for (auto [e, tilemap] : view.each())
			{
				std::vector<Tile> flattenMap;

				// TODO: camera culling
				for (const auto& chunk : tilemap.worldData)
				{
					for(const auto& tile : chunk.chunks)
						flattenMap.push_back(tile);
				}
				
				mTilemapRenderer.set_atlas_texture(tilemap.textureHandle);
				mTilemapRenderer.set_tile_data(flattenMap);
				mTilemapRenderer.set_accumulated_time(time.get_accumulated_milliseconds());
			}
		}

	public:
		TilemapRenderer& renderer() { return mTilemapRenderer; }

	private:
		NON_OWNING Scene* rScene = nullptr;
		TilemapRenderer mTilemapRenderer;
	};
}