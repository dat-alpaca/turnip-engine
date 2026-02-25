#pragma once
#include "memory/memory.hpp"
#include "view/view.hpp"
#include "graphics/components.hpp"
#include "graphics/renderer/tilemap_renderer.hpp"
#include "rhi/rhi.hpp"
#include "scene/common_components.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"
#include "utils/time/time.hpp"

namespace tur
{
	class TilemapView : public View
	{
	public:
		TilemapView(NON_OWNING RenderInterface* rhi)
		{
			mTilemapRenderer.initialize(rhi);
			// mTilemapRenderer.set_clear_color({}, ClearFlags::COLOR | ClearFlags::DEPTH);
		}

	public:
		void render_deferred()
		{ 
			mTilemapRenderer.render();
		}
    
    public:
        void on_event(Event& event) override
		{
			Subscriber subscriber(event);
			subscriber.subscribe<SceneCreatedEvent>([this](const SceneCreatedEvent& event) -> bool {
				auto& registry = event.scene->get_registry();
				registry.on_construct<Tilemap2DComponent>().connect<&TilemapView::on_tilemap_component_added>(this);
				return false;
			});

			subscriber.subscribe<SceneSwitchedEvent>([this](const SceneSwitchedEvent& event) -> bool {
				rScene = event.currentScene;
				return false;
			});

			subscriber.subscribe<CameraSwitchedEvent>([this](const CameraSwitchedEvent& event) -> bool {
				mTilemapRenderer.set_camera(event.camera);
				return false;
			});

			subscriber.subscribe<WindowFramebufferEvent>(
				[&](const WindowFramebufferEvent& resizeEvent) -> bool
				{
					mTilemapRenderer.set_viewport({0.f, 0.f, (f32)resizeEvent.width, (f32)resizeEvent.height});
					mTilemapRenderer.set_scissor({0, 0, resizeEvent.width, resizeEvent.height});

					return false;
				}
			);
		}

		void on_update(const Time& time) override
		{
			auto view = rScene->get_registry().view<Tilemap2DComponent, const CullingComponent>();
			for (auto [e, tilemap, culling] : view.each())
			{
				if(!culling.visible)
					continue;

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

	private:
		void on_tilemap_component_added(entt::registry& registry, entt::entity entity)
		{
			registry.emplace<CullingComponent>(entity);
			registry.emplace<TransformComponent>(entity);
		}

	public:
		TilemapRenderer& renderer() { return mTilemapRenderer; }

	private:
		NON_OWNING Scene* rScene = nullptr; 
		TilemapRenderer mTilemapRenderer;
	};
}