#pragma once
#include <vector>

#include "assets/asset.hpp"
#include "glm/fwd.hpp"
#include "defines.hpp"
#include "graphics/objects/material.hpp"
#include "graphics/objects/tile.hpp"
#include "graphics/objects/texture.hpp"
#include "graphics/objects/buffer.hpp"

namespace tur
{
	struct CullingComponent
	{
		REGISTER_COMPONENT(culling);

	public:
		CullingComponent() = default;
		
	public:
		FIELD_SET_INTERNALLY bool visible = true;
	};

	struct Sprite2DComponent
	{
		REGISTER_COMPONENT(sprite2D);

	public:
		Sprite2DComponent() = default;
		Sprite2DComponent(asset_handle assetHandle)
			: assetHandle(assetHandle)
		{
		}

	public:
		FIELD_SET_INTERNALLY asset_handle assetHandle = invalid_handle;
		FIELD_SET_INTERNALLY texture_handle textureHandle = invalid_handle;
		
		uuid textureUUID;
	};

	struct CubemapComponent
	{
		REGISTER_COMPONENT(cubemap);

	public:
		CubemapComponent() = default;
		CubemapComponent(asset_handle assetHandle)
			: assetHandle(assetHandle)
		{
		}

	public:
		FIELD_SET_INTERNALLY asset_handle assetHandle = invalid_handle;
		FIELD_SET_INTERNALLY texture_handle textureHandle = invalid_handle;
		
		uuid cubemapTextureUUID;
	};

	struct Tilemap2DComponent
	{
		REGISTER_COMPONENT(tilemap2D);

	public:
		Tilemap2DComponent() = default;
		Tilemap2DComponent(asset_handle assetHandle, u32 tilePixelSize)
			: assetHandle(assetHandle)
			, tilePixelSize(tilePixelSize)
		{
		}

	public:
		std::vector<TilemapChunk> worldData;
		u32 tilesPerChunk;
		u32 tilePixelSize;

		uuid tilemapTextureUUID;

		FIELD_SET_INTERNALLY asset_handle assetHandle = invalid_handle;
		FIELD_SET_INTERNALLY texture_handle textureHandle = invalid_handle;
	};

	struct TextComponent
	{
		REGISTER_COMPONENT(text);

	public:
		TextComponent() = default;
		TextComponent(asset_handle assetHandle)
			: assetHandle(assetHandle)
		{
		}

	public:
		FIELD_SET_INTERNALLY asset_handle assetHandle = invalid_handle;
		FIELD_SET_INTERNALLY texture_handle textureHandle = invalid_handle;

		glm::vec4 color;
		std::string text;
		uuid fontUUID;
	};

	struct MeshComponent
	{
		REGISTER_COMPONENT(mesh);

	public:
		MeshComponent() = default;
		MeshComponent(asset_handle assetHandle)
			: assetHandle(assetHandle)
		{
		}

	public:
		uuid meshUUID;

	public:
		FIELD_SET_INTERNALLY MetallicRoughnessMaterial material;

		FIELD_SET_INTERNALLY asset_handle assetHandle = invalid_handle;

		FIELD_SET_INTERNALLY buffer_handle vbo = invalid_handle;
		FIELD_SET_INTERNALLY buffer_handle ebo = invalid_handle;
		FIELD_SET_INTERNALLY u64 indexCount = 0;
		FIELD_SET_INTERNALLY BufferIndexType indexType = BufferIndexType::UNSIGNED_BYTE;
	};
}