#pragma once
#include "assets/asset.hpp"
#include "graphics/objects/texture.hpp"

namespace tur::asset
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
}