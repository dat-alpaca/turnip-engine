#pragma once
#include "core/assets/asset.hpp"
#include "core/event/event.hpp"

namespace tur
{
	struct AssetUnloadedEvent : public Event
	{
		DEFINE_EVENT(EventType::ASSET_UNLOADED);

	public:
		AssetUnloadedEvent(asset_handle assetHandle)
			: assetHandle(assetHandle)
		{
		}

	public:
		asset_handle assetHandle;
	};
}