#pragma once
#include "assets/asset.hpp"
#include "assets/texture/texture_asset.hpp"

namespace tur
{
	struct CubemapAsset : public Asset
	{
	public:
		CubemapAsset() { metadata.type = AssetType::CUBEMAP; }

    public:
		std::array<std::filesystem::path, 6> filepaths;
		std::vector<byte> data;
		u32 width = 0;
		u32 height = 0;
		u32 channels = 0;

	public:
		TextureOptions options = {};
	};
}