#pragma once
#include "assets/asset.hpp"
#include "assets/texture/texture_options.hpp"

namespace tur
{
	struct CubemapAsset : public Asset
	{
	public:
		CubemapAsset() { metadata.type = AssetType::CUBEMAP; }

    public:
		FIELD_SET_INTERNALLY std::vector<byte> data;
		std::array<std::filesystem::path, 6> filepaths;
		TextureOptions options = {};
		
		u32 width = 0;
		u32 height = 0;
		u32 channels = 0;
	};
}