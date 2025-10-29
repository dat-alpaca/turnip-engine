#pragma once
#include "asset.hpp"
#include "graphics/objects/texture.hpp"
#include "utils/uuid/uuid.hpp"
#include <filesystem>

namespace tur
{
	struct TextureAsset : public Asset
	{
	public:
		TextureAsset() { metadata.type = AssetType::TEXTURE; }

	public:
		std::vector<byte> data;

		u32 width = 0;
		u32 height = 0;
		u32 channels = 0;

		TextureDataFormat dataFormat = TextureDataFormat::RGBA;
		bool floatTexture = false;
	};

	struct TextureLoadResult
	{
		asset_handle assetHandle;
		texture_handle textureHandle;
	};
}
