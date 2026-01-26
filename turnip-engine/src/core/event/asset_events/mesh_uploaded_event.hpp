#pragma once
#include "defines.hpp"
#include "event/event.hpp"
#include "assets/asset.hpp"
#include "graphics/objects/buffer.hpp"
#include "graphics/objects/material.hpp"

namespace tur
{
	struct MeshUploadData
	{
		asset_handle meshAssetHandle;
		
		buffer_handle vbo;
		buffer_handle ebo;
		u64 indexCount;
		BufferIndexType indexType;

		MetallicRoughnessMaterial material;
	};

	struct MeshUploadedEvent : public Event
	{
		DEFINE_EVENT(EventType::MESH_UPLOADED);

	public:
		MeshUploadedEvent(const MeshUploadData& data)
			: data(data)
		{
			TUR_ASSERT(data.meshAssetHandle != invalid_handle, "Invalid mesh asset handle uploaded");
			TUR_ASSERT(data.vbo != invalid_handle, "Invalid vertex buffer handle uploaded");
			TUR_ASSERT(data.ebo != invalid_handle, "Invalid index buffer handle uploaded");
			TUR_ASSERT(data.indexCount >= 0, "Invalid index count uploaded");
		}

	public:
		MeshUploadData data;
	};
}