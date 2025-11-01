#pragma once
#include "assets/asset.hpp"
#include "graphics/objects/texture.hpp"
#include "texture_options.hpp"

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

	public:
		TextureOptions options;
	};
}