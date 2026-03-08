#pragma once
#include "defines.hpp"
#include "event/event.hpp"
#include "assets/asset.hpp"
#include "graphics/objects/buffer.hpp"

namespace tur
{
	struct MeshUploadData
	{
		asset_handle meshAssetHandle;		
		BufferIndexType indexType;
		u64 indexCount;
	};

	struct MeshUploadedEvent : public Event
	{
		DEFINE_EVENT(EventType::MESH_UPLOADED);

	public:
		MeshUploadedEvent(const MeshUploadData& data)
			: data(data)
		{
			TUR_ASSERT(data.meshAssetHandle != invalid_handle, "Invalid mesh asset handle uploaded");
			TUR_ASSERT(data.indexCount >= 0, "Invalid index count uploaded");
		}

	public:
		MeshUploadData data;
	};
}