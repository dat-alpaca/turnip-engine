#pragma once
#include <vector>

#include "assets/asset.hpp"
#include "graphics/camera.hpp"
#include "defines.hpp"
#include "graphics/objects/material.hpp"
#include "graphics/objects/tile.hpp"
#include "graphics/objects/texture.hpp"
#include "graphics/objects/buffer.hpp"

#define FIELD_SET_INTERNALLY

namespace tur
{
	struct CameraComponent
	{
	public:
		CameraComponent()
		{
			camera.position = glm::vec3(0.0f);
			camera.target = glm::vec3(0.0f, 0.0f, -1.0f);
		}
		CameraComponent(const Camera& camera)
			: camera(camera)
		{
		}

	public:
		bool mainCamera = false;
		Camera camera;
	};

	struct CullingComponent
	{
	public:
		CullingComponent() = default;
		
	public:
		FIELD_SET_INTERNALLY bool visible = true;
	};

	struct Sprite2DComponent
	{
	public:
		Sprite2DComponent() = default;
		Sprite2DComponent(asset_handle assetHandle)
			: assetHandle(assetHandle)
		{
		}

	public:
		FIELD_SET_INTERNALLY asset_handle assetHandle = invalid_handle;
		FIELD_SET_INTERNALLY texture_handle textureHandle = invalid_handle;
		
		UUID uuid;
	};

	struct Tilemap2DComponent
	{
	public:
		Tilemap2DComponent() = default;
		Tilemap2DComponent(asset_handle assetHandle, u32 tilePixelSize)
			: assetHandle(assetHandle)
			, tilePixelSize(tilePixelSize)
		{
		}

	public:
		FIELD_SET_INTERNALLY std::vector<TilemapChunk> worldData;
		FIELD_SET_INTERNALLY u32 tilesPerChunk;
		FIELD_SET_INTERNALLY u32 tilePixelSize;

		FIELD_SET_INTERNALLY asset_handle assetHandle = invalid_handle;
		FIELD_SET_INTERNALLY texture_handle textureHandle = invalid_handle;

		UUID uuid;
	};

	struct MeshComponent
	{
	public:
		MeshComponent() = default;
		MeshComponent(asset_handle assetHandle)
			: assetHandle(assetHandle)
		{
		}

	public:
		FIELD_SET_INTERNALLY asset_handle assetHandle = invalid_handle;
		UUID uuid;

	public:
		FIELD_SET_INTERNALLY MetallicRoughnessMaterial material;

		FIELD_SET_INTERNALLY buffer_handle vbo = invalid_handle;
		FIELD_SET_INTERNALLY buffer_handle ebo = invalid_handle;
		FIELD_SET_INTERNALLY u64 indexCount = 0;
	};
}