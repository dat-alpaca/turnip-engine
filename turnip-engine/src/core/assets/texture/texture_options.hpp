#pragma once
#include "common.hpp"
#include "graphics/objects/texture.hpp"

namespace tur
{
	struct TextureOptions
	{
		// Texture Asset Options:
		TextureDataFormat dataFormat = TextureDataFormat::RGBA;
		bool floatTexture = false;

		u32 layerWidth = 0;
		u32 layerHeight = 0;
		u32 depth = 1;
		u32 mipLevels = 1;
		u32 samples = 1;
		u32 arrayLayers = 1;

		// Texture Options:
		TextureFormat format = TextureFormat::RGBA8_UNORM;
		TextureType type = TextureType::TEXTURE_2D;

		WrapMode wrapS = WrapMode::CLAMP_TO_BORDER;
		WrapMode wrapT = WrapMode::CLAMP_TO_BORDER;
		WrapMode wrapR = WrapMode::CLAMP_TO_BORDER;
		FilterMode minFilter = FilterMode::NEAREST;
		FilterMode magFilter = FilterMode::NEAREST;

		TextureTiling tiling = TextureTiling::OPTIMAL;
		bool generateMipmaps = false;
		bool isTextureArray = false;
	};
}