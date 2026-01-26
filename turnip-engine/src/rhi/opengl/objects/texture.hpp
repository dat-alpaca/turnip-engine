#pragma once
#include "graphics/objects/texture.hpp"
#include <glad/gl.h>

namespace tur::gl
{
	struct Texture
	{
		TextureDescriptor descriptor;
		gl_handle handle = invalid_handle;
	};
}

namespace tur::gl
{
	constexpr static gl_handle get_texture_type(TextureType type)
	{
		switch (type)
		{
			case TextureType::TEXTURE_2D:
				return GL_TEXTURE_2D;

			case TextureType::TEXTURE_3D:
				return GL_TEXTURE_3D;

			case TextureType::ARRAY_TEXTURE_2D:
				return GL_TEXTURE_2D_ARRAY;

			case TextureType::CUBE_MAP:
				return GL_TEXTURE_CUBE_MAP;
		}

		TUR_LOG_ERROR("Invalid Texture Type: {}. Default: GL_TEXTURE_2D", static_cast<int>(type));

		return GL_TEXTURE_2D;
	}

	constexpr static gl_handle get_wrap_mode(WrapMode mode)
	{
		switch (mode)
		{
			case WrapMode::REPEAT:
				return GL_REPEAT;

			case WrapMode::MIRRORED_REPEAT:
				return GL_MIRRORED_REPEAT;

			case WrapMode::CLAMP_TO_EDGE:
				return GL_CLAMP_TO_EDGE;

			case WrapMode::CLAMP_TO_BORDER:
				return GL_CLAMP_TO_BORDER;
		}

		TUR_LOG_ERROR("Invalid Texture Wrap Mode: {}. Default: GL_REPEAT", static_cast<int>(mode));

		return GL_REPEAT;
	}

	constexpr static gl_handle get_filter_mode(FilterMode mode, bool isMinFilter)
	{
		if (isMinFilter)
		{
			switch (mode)
			{
				case FilterMode::NEAREST:
					return GL_NEAREST;

				case FilterMode::LINEAR:
					return GL_LINEAR;

				case FilterMode::BILINEAR:
					return GL_LINEAR_MIPMAP_NEAREST;

				case FilterMode::TRILINEAR:
					return GL_LINEAR_MIPMAP_LINEAR;
			}
		}
		else
		{
			switch (mode)
			{
				case FilterMode::NEAREST:
					return GL_NEAREST;

				case FilterMode::LINEAR:
					return GL_LINEAR;

				case FilterMode::BILINEAR:
					return GL_LINEAR;

				case FilterMode::TRILINEAR:
					return GL_LINEAR;
			}
		}

		TUR_LOG_ERROR("Invalid Texture Wrap Mode: {}. Default: GL_NEAREST", static_cast<int>(mode));

		return GL_REPEAT;
	}

	constexpr static gl_handle get_texture_format(TextureFormat format)
	{
		switch (format)
		{
			case TextureFormat::DEPTH_16_UNORM:
				return GL_DEPTH_COMPONENT;

			case TextureFormat::DEPTH_STENCIL24_S8U_INT:
				return GL_DEPTH_STENCIL;

			case TextureFormat::R8_UNORM:
				return GL_R8;

			case TextureFormat::RG8_UNORM:
				return GL_RG8;

			case TextureFormat::RGB8_UNORM:
				return GL_RGB8;

			case TextureFormat::RGBA8_UNORM:
				return GL_RGBA8;

			case TextureFormat::B8G8R8A8_UNORM:
				return GL_BGRA;
		}

		TUR_LOG_ERROR("Invalid Texture Format: {}. Default: GL_RGBA", static_cast<int>(format));

		return GL_RGBA;
	}

	constexpr static gl_handle get_texture_data_format(TextureDataFormat format)
	{
		switch (format)
		{
			case TextureDataFormat::RED:
				return GL_RED;

			case TextureDataFormat::RG:
				return GL_RG;

			case TextureDataFormat::RGB:
				return GL_RGB;

			case TextureDataFormat::RGBA:
				return GL_RGBA;

			case TextureDataFormat::BGR:
				return GL_BGR;

			case TextureDataFormat::BGRA:
				return GL_BGRA;

			case TextureDataFormat::RED_INTEGER:
				return GL_RED_INTEGER;

			case TextureDataFormat::RG_INTEGER:
				return GL_RG_INTEGER;

			case TextureDataFormat::RGB_INTEGER:
				return GL_RGB_INTEGER;

			case TextureDataFormat::RGBA_INTEGER:
				return GL_RGBA_INTEGER;

			case TextureDataFormat::BGR_INTEGER:
				return GL_BGR_INTEGER;

			case TextureDataFormat::BGRA_INTEGER:
				return GL_BGRA_INTEGER;
		}

		TUR_LOG_ERROR("Invalid Texture Format: {}. Default: GL_RGBA", static_cast<int>(format));

		return GL_RGBA;
	}
}