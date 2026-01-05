#pragma once
#include "event/event.hpp"
#include "assets/asset.hpp"
#include "graphics/objects/buffer.hpp"

namespace tur
{
	struct ModelUploadedEvent : public Event
	{
		DEFINE_EVENT(EventType::MODEL_UPLOADED);

	public:
		ModelUploadedEvent(asset_handle assetHandle, buffer_handle vbo, buffer_handle ebo, u64 indexCount)
			: assetHandle(assetHandle)
			, vbo(vbo)
			, ebo(ebo)
			, indexCount(indexCount)
		{
		}

	public:
		asset_handle assetHandle;
		buffer_handle vbo;
		buffer_handle ebo;
		u64 indexCount;
	};
}