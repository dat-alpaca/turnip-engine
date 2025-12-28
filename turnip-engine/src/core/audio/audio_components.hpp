#pragma once
#include "assets/asset.hpp"

namespace tur
{
	struct AudioSourceComponent
	{
	public:
		AudioSourceComponent() = default;
		AudioSourceComponent(asset_handle assetHandle)
			: assetHandle(assetHandle)
		{
		}

	public:
		asset_handle assetHandle = invalid_handle;
		std::filesystem::path filepath;
	};
}