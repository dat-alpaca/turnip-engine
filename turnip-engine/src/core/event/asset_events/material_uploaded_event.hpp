#pragma once
#include "event/event.hpp"
#include "assets/asset.hpp"
#include "graphics/objects/material.hpp"

namespace tur
{
	struct MaterialUploadedEvent : public Event
	{
		DEFINE_EVENT(EventType::MATERIAL_UPLOADED);

	public:
		MaterialUploadedEvent(asset_handle assetHandle, material_handle materialHandle)
			: assetHandle(assetHandle)
			, materialHandle(materialHandle)
		{
		}

	public:
		asset_handle assetHandle;
		material_handle materialHandle;
	};
}