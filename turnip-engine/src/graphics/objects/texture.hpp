#pragma once
#include "common.hpp"

namespace tur
{
	using texture_handle = handle_type;

	enum class TextureType
	{
		TEXTURE_2D,
		TEXTURE_3D,
		ARRAY_TEXTURE_2D,
		CUBE_MAP,
	};

	enum class WrapMode
	{
		REPEAT,
		MIRRORED_REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER
	};

	enum class FilterMode
	{
		NEAREST,
		LINEAR,
		BILINEAR,
		TRILINEAR
	};

	// Represents a texture on the GPU
	enum class TextureFormat
	{
		R8_UNORM,
		RG8_UNORM,
		RGB8_UNORM,
		RGBA8_UNORM,
		RGBA16_SFLOAT,

		DEPTH_16_UNORM,
		DEPTH_STENCIL16_S8U_INT,

		B8G8R8A8_UNORM
	};

	// Represents data formats and is used for loading textures, rather than representing them on the GPU
	enum class TextureDataFormat
	{
		RED,
		RG,
		RGB,
		RGBA,
		BGR,
		BGRA,

		RED_INTEGER,
		RG_INTEGER,
		RGB_INTEGER,
		RGBA_INTEGER,
		BGR_INTEGER,
		BGRA_INTEGER,
	};

	constexpr static inline const char* get_texture_data_format_name(TextureDataFormat dataFormat)
	{
		switch (dataFormat)
		{
			case TextureDataFormat::RED:
				return "RED";

			case TextureDataFormat::RG:
				return "RG";

			case TextureDataFormat::RGB:
				return "RGB";

			case TextureDataFormat::RGBA:
				return "RGBA";

			case TextureDataFormat::BGR:
				return "BGR";

			case TextureDataFormat::BGRA:
				return "BGRA";

			case TextureDataFormat::RED_INTEGER:
				return "RED INTEGER";

			case TextureDataFormat::RG_INTEGER:
				return "RG INTEGER";

			case TextureDataFormat::RGB_INTEGER:
				return "RGB INTEGER";

			case TextureDataFormat::RGBA_INTEGER:
				return "RGBA INTEGER";

			case TextureDataFormat::BGR_INTEGER:
				return "BGR INTEGER";

			case TextureDataFormat::BGRA_INTEGER:
				return "BGRA INTEGER";
		}

		return "UNKNOWN";
	}

	struct TextureDescriptor
	{
		// TODO: array layers.

		u32 width = 0;
		u32 height = 0;
		u32 depth = 1;
		u32 mipLevels = 1;
		u32 samples = 1;

		TextureFormat format = TextureFormat::B8G8R8A8_UNORM;
		TextureType type = TextureType::TEXTURE_2D;

		bool generateMipmaps = false;

		WrapMode wrapS = WrapMode::CLAMP_TO_BORDER;
		WrapMode wrapT = WrapMode::CLAMP_TO_BORDER;
		WrapMode wrapR = WrapMode::CLAMP_TO_BORDER;
		FilterMode minFilter = FilterMode::NEAREST;
		FilterMode magFilter = FilterMode::NEAREST;
	};
}