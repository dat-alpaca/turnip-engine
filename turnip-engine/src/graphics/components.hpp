#pragma once
#include <vector>

#include "assets/asset.hpp"
#include "graphics/camera.hpp"
#include "defines.hpp"
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
		bool visible = true;
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
		asset_handle assetHandle = invalid_handle;
		texture_handle textureHandle = invalid_handle;
		
		std::filesystem::path filepath;
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
		std::vector<TilemapChunk> worldData;
		u32 tilesPerChunk;
		u32 tilePixelSize;

		asset_handle assetHandle = invalid_handle;
		texture_handle textureHandle = invalid_handle;

		std::filesystem::path filepath;
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
		asset_handle assetHandle = invalid_handle;
		std::filesystem::path filepath;

	public:
		FIELD_SET_INTERNALLY glm::vec4 baseColor;

		FIELD_SET_INTERNALLY buffer_handle vbo = invalid_handle;
		FIELD_SET_INTERNALLY buffer_handle ebo = invalid_handle;
		FIELD_SET_INTERNALLY texture_handle albedo = invalid_handle;
		FIELD_SET_INTERNALLY u64 indexCount = 0;
	};
}