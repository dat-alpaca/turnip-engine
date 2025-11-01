#pragma once
#include "assets/asset.hpp"
#include "graphics/objects/texture.hpp"

namespace tur
{
	struct Sprite2DComponent
	{
	public:
		Sprite2DComponent() = default;
		Sprite2DComponent(asset_handle assetHandle)
			: assetHandle(assetHandle)
		{
		}

	public:
		asset_handle assetHandle = invalid_handle;
		texture_handle textureHandle = invalid_handle;
	};
}