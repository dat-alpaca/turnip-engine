#pragma once
#include <vector>

#include "assets/asset.hpp"
#include "graphics/tile.hpp"
#include "defines.hpp"
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

	struct Tilemap2DComponent
	{
	public:
		Tilemap2DComponent(asset_handle assetHandle, u32 tilePixelSize)
			: assetHandle(assetHandle)
			, tilePixelSize(tilePixelSize)
		{
		}

	public:
		std::vector<TilemapChunk> worldData;
		u32 tilesPerChunk;
		u32 tilePixelSize;

		asset_handle assetHandle = invalid_handle;
		texture_handle textureHandle = invalid_handle;
	};
}