#pragma once
#include "common.hpp"
#include "utils/string_utils.hpp"

namespace tur
{
	using texture_handle = handle_type;

	/* Texture Type */
	enum class TextureType
	{
		TEXTURE_2D,
		TEXTURE_3D,
		ARRAY_TEXTURE_2D,
		CUBE_MAP,
	};

	constexpr static inline const char* get_texture_type_name(TextureType textureType)
	{
		switch (textureType)
		{
			case TextureType::TEXTURE_2D:
				return "TEXTURE2D";

			case TextureType::TEXTURE_3D:
				return "TEXTURE3D";

			case TextureType::ARRAY_TEXTURE_2D:
				return "ARRAYTEXTURE2D";

			case TextureType::CUBE_MAP:
				return "CUBEMAP";
		
			default:
				return "INVALID";
		}
	}
	
	static inline TextureType get_texture_type_value(const std::string& name)
	{
		std::string converted = name;
		to_upper(converted);

        if (converted == "TEXTURE2D")
			return TextureType::TEXTURE_2D;

		if (converted == "TEXTURE3D")
			return TextureType::TEXTURE_3D;

		if (converted == "ARRAYTEXTURE2D")
			return TextureType::ARRAY_TEXTURE_2D;

		if (converted == "CUBEMAP")
			return TextureType::CUBE_MAP;

		TUR_LOG_ERROR("Invalid texture type. Default: Texture 2D");
		return TextureType::TEXTURE_2D;
	}

	/* Wrap Mode */
	enum class WrapMode
	{
		REPEAT,
		MIRRORED_REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER
	};

	constexpr static inline const char* get_wrap_mode_name(WrapMode wrapMode)
	{
		switch (wrapMode)
		{
			case WrapMode::REPEAT:
				return "REPEAT";

			case WrapMode::MIRRORED_REPEAT:
				return "MIRROREDREPEAT";

			case WrapMode::CLAMP_TO_EDGE:
				return "CLAMPTOEDGE";

			case WrapMode::CLAMP_TO_BORDER:
				return "CLAMPTOBORDER";
		
			default:
				return "INVALID";
		}
	}

	static inline WrapMode get_wrap_mode_value(const std::string& name)
	{
		std::string converted = name;
		to_upper(converted);

        if (converted == "REPEAT")
			return WrapMode::REPEAT;

		if (converted == "MIRROREDREPEAT")
			return WrapMode::MIRRORED_REPEAT;

		if (converted == "CLAMPTOEDGE")
			return WrapMode::CLAMP_TO_EDGE;

		if (converted == "CLAMPTOBORDER")
			return WrapMode::CLAMP_TO_BORDER;

		TUR_LOG_ERROR("Invalid wrap mode. Default: repeat");
		return WrapMode::REPEAT;
	}

	/* Filter Mode */
	enum class FilterMode
	{
		NEAREST,
		LINEAR,
		BILINEAR,
		TRILINEAR
	};

	constexpr static inline const char* get_filter_mode_name(FilterMode filterMode)
	{
		switch (filterMode)
		{
			case FilterMode::NEAREST:
				return "NEAREST";

			case FilterMode::LINEAR:
				return "LINEAR";

			case FilterMode::BILINEAR:
				return "BILINEAR";

			case FilterMode::TRILINEAR:
				return "TRILINEAR";
		
			default:
				return "INVALID";
		}
	}

	static inline FilterMode get_filter_mode_value(const std::string& name)
	{
		std::string converted = name;
		to_upper(converted);

        if (converted == "NEAREST")
			return FilterMode::NEAREST;

		if (converted == "LINEAR")
			return FilterMode::LINEAR;

		if (converted == "BILINEAR")
			return FilterMode::BILINEAR;

		if (converted == "TRILINEAR")
			return FilterMode::TRILINEAR;

		TUR_LOG_ERROR("Invalid filter mode. Default: nearest");
		return FilterMode::NEAREST;
	}

	/* Texture Tiling */
	enum class TextureTiling
	{
		OPTIMAL,
		RAW
	};

	constexpr static inline const char* get_texture_tiling_name(TextureTiling textureTiling)
	{
		switch (textureTiling)
		{
			case TextureTiling::OPTIMAL:
				return "OPTIMAL";

			case TextureTiling::RAW:
				return "RAW";
		
			default:
				return "INVALID";
		}
	}

	static inline TextureTiling get_texture_tiling_value(const std::string& name)
	{
		std::string converted = name;
		to_upper(converted);

        if (converted == "RAW")
			return TextureTiling::RAW;

		if (converted == "OPTIMAL")
			return TextureTiling::OPTIMAL;

		TUR_LOG_ERROR("Invalid texture tiling. Default: optimal");
		return TextureTiling::OPTIMAL;
	}

	/* Texture Format */
	// Represents a texture on the GPU
	enum class TextureFormat
	{
		R8_UNORM,
		RG8_UNORM,
		RGB8_UNORM,
		RGBA8_UNORM,
		RGBA16_SFLOAT,
		DEPTH_16_UNORM,
		DEPTH_STENCIL24_S8U_INT,
		B8G8R8A8_UNORM
	};

	constexpr static inline const char* get_texture_format_name(TextureFormat textureFormat)
	{
		switch (textureFormat)
		{
			case TextureFormat::R8_UNORM: 			     return "R8U";
			case TextureFormat::RG8_UNORM: 			     return "RG8U";
			case TextureFormat::RGB8_UNORM: 		     return "RGB8U";
			case TextureFormat::RGBA8_UNORM: 		     return "RGBA8U";
			case TextureFormat::RGBA16_SFLOAT: 		     return "RGBA16S";
			case TextureFormat::DEPTH_16_UNORM: 		 return "DEPTH16U";
			case TextureFormat::DEPTH_STENCIL24_S8U_INT: return "DEPTHSTENCIL24S8U";
			case TextureFormat::B8G8R8A8_UNORM:          return "B8G8R8A8U";
		
			default:
				return "INVALID";
		}
	}

	static inline TextureFormat get_texture_format_value(const std::string& name)
	{
		std::string converted = name;
		to_upper(converted);

    	if (converted == "R8U")
			return TextureFormat::R8_UNORM;

    	if (converted == "RG8U")
			return TextureFormat::RG8_UNORM;

    	if (converted == "RGB8U")
			return TextureFormat::RGB8_UNORM;

    	if (converted == "RGBA8U")
			return TextureFormat::RGBA8_UNORM;

		if (converted == "RGBA16S")
			return TextureFormat::RGBA16_SFLOAT;

        if (converted == "DEPTH16U")
			return TextureFormat::DEPTH_16_UNORM;

        if (converted == "DEPTHSTENCIL24S8U")
			return TextureFormat::DEPTH_STENCIL24_S8U_INT;

        if (converted == "B8G8R8A8U")
			return TextureFormat::B8G8R8A8_UNORM;
		
		TUR_LOG_ERROR("Invalid texture tiling. Default: RGBA8Unorm");
		return TextureFormat::RGBA8_UNORM;
	}

	inline u32 get_texture_format_byte_size(TextureFormat format)
	{
		switch(format)
		{
			case TextureFormat::R8_UNORM: 
				return sizeof(u8);

			case TextureFormat::RG8_UNORM:
				return sizeof(u8) * 2;

			case TextureFormat::RGB8_UNORM:
				return sizeof(u8) * 3;

			case TextureFormat::RGBA8_UNORM:
				return sizeof(u8) * 4;

			case TextureFormat::RGBA16_SFLOAT:
				return sizeof(float) * 4;

			case TextureFormat::DEPTH_16_UNORM:
				return sizeof(u16);

			case TextureFormat::DEPTH_STENCIL24_S8U_INT:
				return sizeof(u32);

			case TextureFormat::B8G8R8A8_UNORM:
				return sizeof(u32);
		}
		
		return 0;
	}

	/* Texture Data Format */
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
	
	static inline TextureDataFormat get_texture_data_format_value(const std::string& name)
	{
		std::string converted = name;
		to_upper(converted);

		if (converted == "RED") 
			return TextureDataFormat::RED;

		if (converted == "RG") 
			return TextureDataFormat::RG;

		if (converted == "RGB") 
			return TextureDataFormat::RGB;

		if (converted == "RGBA") 
			return TextureDataFormat::RGBA;

		if (converted == "BGR") 
			return TextureDataFormat::BGR;

		if (converted == "BGRA") 
			return TextureDataFormat::BGRA;

		if (converted == "REDINT") 
			return TextureDataFormat::RED_INTEGER;

		if (converted == "RGINT") 
			return TextureDataFormat::RG_INTEGER;

		if (converted == "RGBINT") 
			return TextureDataFormat::RGB_INTEGER;

		if (converted == "RGBAINT") 
			return TextureDataFormat::RGBA_INTEGER;

		if (converted == "BGRINT") 
			return TextureDataFormat::BGR_INTEGER;

		if (converted == "BGRAINT") 
			return TextureDataFormat::BGRA_INTEGER;

		TUR_LOG_ERROR("Invalid texture data format. Default: RGBA");
		return TextureDataFormat::RGBA;
	}

	constexpr static inline const char* get_texture_data_format_name(TextureDataFormat textureDataFormat)
	{
		switch (textureDataFormat)
		{
			case TextureDataFormat::RED: 			    return "RED";
			case TextureDataFormat::RG: 			    return "RG";
			case TextureDataFormat::RGB: 		     	return "RGB";
			case TextureDataFormat::RGBA: 		     	return "RGBA";
			case TextureDataFormat::BGR: 		     	return "BGR";
			case TextureDataFormat::BGRA: 		 		return "BGRA";
			case TextureDataFormat::RED_INTEGER: 		return "REDINT";
			case TextureDataFormat::RG_INTEGER:         return "RGINT";
			case TextureDataFormat::RGB_INTEGER:        return "RGBINT";
			case TextureDataFormat::RGBA_INTEGER:       return "RGBAINT";
			case TextureDataFormat::BGR_INTEGER:        return "BGRINT";
			case TextureDataFormat::BGRA_INTEGER:       return "BGRAINT";
		
			default:
				return "INVALID";
		}
	}

	struct TextureDescriptor
	{
		// TODO: array layers.

		u32 width = 0;
		u32 height = 0;
		u32 depth = 1;
		u32 mipLevels = 1;
		u32 samples = 1;
		u32 arrayLayers = 1;
		u32 layerWidth = 0;
		u32 layerHeight = 0;

		TextureFormat format = TextureFormat::RGBA8_UNORM;
		TextureType type = TextureType::TEXTURE_2D;

		WrapMode wrapS = WrapMode::CLAMP_TO_BORDER;
		WrapMode wrapT = WrapMode::CLAMP_TO_BORDER;
		WrapMode wrapR = WrapMode::CLAMP_TO_BORDER;
		FilterMode minFilter = FilterMode::NEAREST;
		FilterMode magFilter = FilterMode::NEAREST;

		TextureTiling tiling = TextureTiling::OPTIMAL;

		bool generateMipmaps = false;
		bool isColorAttachment = false;
		bool isDepthStencilAttachment = false;
	};
}