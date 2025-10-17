#pragma once
#include "core/event/event.hpp"
#include "graphics/objects/texture.hpp"

namespace tur
{
	struct TextureUploadedEvent : public Event
	{
		DEFINE_EVENT(EventType::TEXTURE_UPLOADED);

	public:
		TextureUploadedEvent(texture_handle textureHandle)
			: textureHandle(textureHandle)
		{
		}

	public:
		texture_handle textureHandle;
	};
}