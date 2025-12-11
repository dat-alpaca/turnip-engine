#pragma once
#include <vector>

#include "assets/asset.hpp"
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

	struct Tile
	{
	public:
		glm::vec2 position;
		u32 layer;
	};

	using tile_handle = handle_type;

	struct Tilemap2DComponent
	{
	public:
		Tilemap2DComponent(asset_handle assetHandle, u32 tileSize)
			: assetHandle(assetHandle)
			, tileSize(tileSize)
		{
		}

	public:
		std::vector<Tile> tileData;
		u32 tileSize;

		asset_handle assetHandle = invalid_handle;
		texture_handle textureHandle = invalid_handle;
	};
}