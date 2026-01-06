#pragma once
#include "defines.hpp"
#include "event/event.hpp"
#include "assets/asset.hpp"
#include "graphics/objects/buffer.hpp"
#include "graphics/objects/material.hpp"

namespace tur
{
	struct ModelUploadData
	{
		asset_handle modelAssetHandle;
		
		buffer_handle vbo;
		buffer_handle ebo;
		u64 indexCount;

		MetallicRoughnessMaterial material;
	};

	struct ModelUploadedEvent : public Event
	{
		DEFINE_EVENT(EventType::MODEL_UPLOADED);

	public:
		ModelUploadedEvent(const ModelUploadData& data)
			: data(data)
		{
			TUR_ASSERT(data.modelAssetHandle != invalid_handle, "Invalid model asset handle uploaded");
			TUR_ASSERT(data.vbo != invalid_handle, "Invalid vertex buffer handle uploaded");
			TUR_ASSERT(data.ebo != invalid_handle, "Invalid index buffer handle uploaded");
			TUR_ASSERT(data.indexCount >= 0, "Invalid index count uploaded");
		}

	public:
		ModelUploadData data;
	};
}