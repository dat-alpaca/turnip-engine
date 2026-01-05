#pragma once
#include "event/event.hpp"
#include "assets/asset.hpp"
#include "graphics/objects/texture.hpp"

namespace tur
{
	struct TextureUploadedEvent : public Event
	{
		DEFINE_EVENT(EventType::TEXTURE_UPLOADED);

	public:
		TextureUploadedEvent(asset_handle assetHandle, texture_handle textureHandle, AssetType type)
			: assetHandle(assetHandle)
			, textureHandle(textureHandle)
			, type(type)
		{
		}

	public:
		asset_handle assetHandle;
		texture_handle textureHandle;
		AssetType type;
	};
}