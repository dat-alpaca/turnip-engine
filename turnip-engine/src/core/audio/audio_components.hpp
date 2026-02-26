#pragma once
#include "assets/asset.hpp"
#include "defines.hpp"

namespace tur
{
	struct AudioSourceComponent
	{
		REGISTER_COMPONENT(audio_source);

	public:
		AudioSourceComponent() = default;
		AudioSourceComponent(asset_handle assetHandle)
			: assetHandle(assetHandle)
		{
		}

	public:
		FIELD_SET_INTERNALLY asset_handle assetHandle = invalid_handle;
		UUID uuid;
	};
}