#pragma once
#include "assets/texture_asset_binder.hpp"
#include "event/events.hpp"
#include "event_consumer.hpp"
#include "event_emitter.hpp"

namespace tur
{
	class TextureAssetBinderAgent : public IEventConsumer, public IEventEmitter
	{
	public:
		void initialize(NON_OWNING TextureAssetBinder* binder) { rBinder = binder; }

	public:
		void on_event(Event& event)
		{
			Subscriber subscriber(event);
			subscriber.subscribe<AssetLoadedEvent>(
				[&](const AssetLoadedEvent& event) -> bool
				{
					auto textureHandle = rBinder->upload_texture(event.assetHandle);

					TextureUploadedEvent uploadEvent(textureHandle);
					callback(uploadEvent);

					return false;
				}
			);
		}

	private:
		NON_OWNING TextureAssetBinder* rBinder = nullptr;
	};
}