#pragma once
#include "assets/asset_library.hpp"
#include "assets/font/font_asset.hpp"
#include "defines.hpp"
#include "graphics/renderer/font_renderer.hpp"
#include "memory/memory.hpp"
#include "utils/transform.hpp"
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
	class FontView : public View
	{
	public:
		FontView(NON_OWNING RenderInterface* rhi, NON_OWNING AssetLibrary* library)
            : rLibrary(library)
		{
			mFontRenderer.initialize(rhi);
			mFontRenderer.set_clear_color({}, ClearFlags::COLOR);
		}

	public:
		void render_deferred()
		{ 
			mFontRenderer.render();
		}
    
    public:
        void on_event(Event& event) override
		{
			Subscriber subscriber(event);
			subscriber.subscribe<SceneCreatedEvent>([this](const SceneCreatedEvent& event) -> bool {
				auto& registry = event.scene->get_registry();
				registry.on_construct<TextComponent>().connect<&FontView::on_text_component_added>(this);
				return false;
			});

			subscriber.subscribe<SceneSwitchedEvent>([this](const SceneSwitchedEvent& event) -> bool {
				rScene = event.currentScene;
				return false;
			});

			subscriber.subscribe<CameraSwitchedEvent>([this](const CameraSwitchedEvent& event) -> bool {
				mFontRenderer.set_camera(event.camera);
				return false;
			});

			subscriber.subscribe<WindowFramebufferEvent>(
				[&](const WindowFramebufferEvent& resizeEvent) -> bool
				{
					mFontRenderer.set_viewport({0.f, 0.f, (f32)resizeEvent.width, (f32)resizeEvent.height});
					mFontRenderer.set_scissor({0, 0, resizeEvent.width, resizeEvent.height});

					return false;
				}
			);
		}

		void on_update(const Time& time) override
		{
			auto view = rScene->get_registry().view<TextComponent, const CullingComponent, const TransformComponent>();
			for (auto [e, text, culling, transform] : view.each())
			{
				if(!culling.visible)
					continue;

                if(text.assetHandle == invalid_handle)
                    continue;

                const FontAsset& fontAsset = rLibrary->get_font_asset(text.assetHandle);
                const auto& worldPosition = transform.worldTransform.position;
                const auto& worldScale = transform.worldTransform.scale;

				std::vector<CharacterEntry> flattenedMap;
                float currentX = worldPosition.x;
				float baselineY = worldPosition.y;
				
				for(char c : text.text)
                {
                    if(c == ' ')
                    {
                        currentX += 0.0f;
                        continue;
                    }

                    auto character = fontAsset.get_character(c);
					if(!character)
						continue;

					float x = currentX + character->bearing.x * worldScale.x;
					float y = baselineY - (character->size.y - character->bearing.y) * worldScale.y;

					float w = character->size.x  * worldScale.x;
    				float h = character->size.y * worldScale.y;

                    flattenedMap.push_back({
                        .position = { x, y, worldPosition.z },
						.dimensions = { w, h },
                        .layer = character->layer
                    });

                    currentX += (character->advance >> 6) * worldScale.x;
                }

				mFontRenderer.set_font_texture(text.textureHandle);
				mFontRenderer.set_character_data(flattenedMap);
			}
		}

	private:
		void on_text_component_added(entt::registry& registry, entt::entity entity)
		{
			registry.emplace<CullingComponent>(entity);

			if(!registry.all_of<TransformComponent>(entity))
				registry.emplace<TransformComponent>(entity);
		}

	public:
		FontRenderer& renderer() { return mFontRenderer; }

	private:
        NON_OWNING AssetLibrary* rLibrary = nullptr;
		NON_OWNING Scene* rScene = nullptr; 
		FontRenderer mFontRenderer;
	};
}