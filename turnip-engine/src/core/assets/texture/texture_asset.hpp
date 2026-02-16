#pragma once
#include "assets/asset.hpp"
#include "texture_options.hpp"

namespace tur
{
	struct TextureAsset : public Asset
	{
	public:
		TextureAsset() { metadata.type = AssetType::TEXTURE; }

	public:
		FIELD_SET_INTERNALLY std::vector<byte> data;
		TextureOptions options = {};
		
		u32 width = 0;
		u32 height = 0;
		u32 channels = 0;
	};
}