#pragma once
#include "assets/texture_asset_binder.hpp"
#include "event/events.hpp"
#include "event_consumer.hpp"

namespace tur
{
	// TODO: add set_event_handle
	// TODO: signal TextureUploaded event
	class TextureAssetBinderConsumer : public IEventConsumer
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
					rBinder->upload_texture(event.assetHandle);
					return false;
				}
			);
		}

	private:
		NON_OWNING TextureAssetBinder* rBinder = nullptr;
	};
}