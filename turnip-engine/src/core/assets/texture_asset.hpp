#pragma once
#include "asset.hpp"
#include "graphics/objects/texture.hpp"
#include "utils/uuid/uuid.hpp"
#include <filesystem>

namespace tur
{
	struct TextureAsset
	{
		std::vector<byte> data;
		AssetMetadata metadata;

		u32 width = 0;
		u32 height = 0;
		u32 depth = 0;
		u32 channels = 0;

		TextureDataFormat dataFormat = TextureDataFormat::RGBA;
		bool floatTexture = false;
	};
}
