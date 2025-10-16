#pragma once
#include "core/assets/asset.hpp"
#include "core/event/event.hpp"

namespace tur
{
	struct AssetLoadedEvent : public Event
	{
		DEFINE_EVENT(EventType::ASSET_LOADED);

	public:
		AssetLoadedEvent(asset_handle assetHandle)
			: assetHandle(assetHandle)
		{
		}

	public:
		asset_handle assetHandle;
	};
}