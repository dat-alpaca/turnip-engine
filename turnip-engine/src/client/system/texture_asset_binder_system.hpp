#pragma once
#include "event/events.hpp"
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
			Subscriber subscriber(event);
			subscriber.subscribe<TextureUploadedEvent>(
				[&](const TextureUploadedEvent& textureUploaded) -> bool
				{
					auto view = rScene->get_registry().view<Sprite2DComponent>();
					for (auto [e, sprite] : view.each())
					{
						if (sprite.assetHandle == textureUploaded.assetHandle)
							sprite.textureHandle = textureUploaded.textureHandle;
					}

					return false;
				}
			);
		}

	private:
		NON_OWNING Scene* rScene = nullptr;
	};
}