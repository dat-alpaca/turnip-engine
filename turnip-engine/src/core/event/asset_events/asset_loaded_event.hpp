#pragma once
#include "core/assets/asset.hpp"
#include "core/event/event.hpp"

namespace tur
{
	struct DefaultAssetLoadedEvent : public Event
	{
		DEFINE_EVENT(EventType::DEFAULT_ASSET_LOADED);

	public:
		DefaultAssetLoadedEvent(asset_handle assetHandle)
			: assetHandle(assetHandle)
		{
		}

	public:
		asset_handle assetHandle;
	};

	struct AssetLoadedEvent : public Event
	{
		DEFINE_EVENT(EventType::ASSET_LOADED);

	public:
		AssetLoadedEvent(asset_handle assetHandle, AssetType type)
			: assetHandle(assetHandle),
			  type(type)
		{
		}

	public:
		asset_handle assetHandle;
		AssetType type;
	};
}