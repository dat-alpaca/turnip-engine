#pragma once
#include "assets/asset.hpp"
#include "event/events.hpp"
#include "graphics/components.hpp"
#include "logging/logging.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"

namespace tur
{
	class TextureAssetBinderSystem
	{
	public:
		void initialize(NON_OWNING Scene* scene) { set_scene(scene); }
		void set_scene(NON_OWNING Scene* scene) { rScene = scene; }

	public:
		void on_event(Event& event)
		{
			TUR_ASS(rScene);

			Subscriber subscriber(event);
			subscriber.subscribe<TextureUploadedEvent>(
				[&](const TextureUploadedEvent& textureUploaded) -> bool
				{
					switch(textureUploaded.type)
					{
						case AssetType::TEXTURE:
							handle_texture(textureUploaded);
							break;

						case AssetType::TEXTURE_ARRAY:
							handle_texture_array(textureUploaded);
							break;
						
						default:
							TUR_LOG_ERROR("Attempted to register an invalid asset type as texture.");
							return false;
					}
					return false;
				}
			);
		}

	private:
		void handle_texture(const TextureUploadedEvent& textureUploaded)
		{
			auto view = rScene->get_registry().view<Sprite2DComponent>();
			for (auto [e, sprite] : view.each())
			{
				if (sprite.assetHandle == textureUploaded.assetHandle)
					sprite.textureHandle = textureUploaded.textureHandle;
			}
		}

		void handle_texture_array(const TextureUploadedEvent& textureUploaded)
		{
			auto view = rScene->get_registry().view<Tilemap2DComponent>();
			for (auto [e, tilemap] : view.each())
			{
				if (tilemap.assetHandle == textureUploaded.assetHandle)
					tilemap.textureHandle = textureUploaded.textureHandle;
			}
		}

	private:
		NON_OWNING Scene* rScene = nullptr;
	};
}