#pragma once
#include "core/event/event.hpp"
#include "graphics/objects/texture.hpp"

namespace tur
{
	struct TextureUploadedEvent : public Event
	{
		DEFINE_EVENT(EventType::TEXTURE_UPLOADED);

	public:
		TextureUploadedEvent(asset_handle assetHandle, texture_handle textureHandle)
			: assetHandle(assetHandle),
			  textureHandle(textureHandle)
		{
		}

	public:
		asset_handle assetHandle;
		texture_handle textureHandle;
	};
}