#pragma once
#include "assets/texture/texture_asset.hpp"
#include "common.hpp"

#include "rhi/opengl/objects/texture.hpp"
#include "rhi/opengl/opengl_common.hpp"
#include <glad/gl.h>

namespace tur::gl
{
	inline Texture allocate_texture(const TextureDescriptor& descriptor)
	{
		gl_handle textureType = get_texture_type(descriptor.type);
		gl_handle textureFormat = get_texture_format(descriptor.format);

		gl_handle textureID;
		glCreateTextures(textureType, 1, &textureID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

		{
			bool generateMipmaps = descriptor.generateMipmaps;

			glTextureParameteri(textureID, GL_TEXTURE_MAX_LEVEL, descriptor.arrayLayers);
			glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, get_wrap_mode(descriptor.wrapS));
			glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, get_wrap_mode(descriptor.wrapT));
			if (descriptor.type == TextureType::TEXTURE_3D)
				glTextureParameteri(textureID, GL_TEXTURE_WRAP_R, get_wrap_mode(descriptor.wrapR));

			glTextureParameteri(
				textureID, GL_TEXTURE_MIN_FILTER, get_filter_mode(descriptor.minFilter, generateMipmaps)
			);
			glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, get_filter_mode(descriptor.magFilter, false));

			if (generateMipmaps)
				glGenerateTextureMipmap(textureID);
		}

		switch (descriptor.type)
		{
			case TextureType::TEXTURE_2D:
				glTextureStorage2D(textureID, descriptor.mipLevels, textureFormat, descriptor.width, descriptor.height);
				break;

			case TextureType::ARRAY_TEXTURE_2D:
				glTextureStorage3D(textureID, descriptor.mipLevels, textureFormat, descriptor.layerWidth, descriptor.layerHeight, descriptor.arrayLayers);
				break;

			case TextureType::CUBE_MAP:
			case TextureType::TEXTURE_3D:
				glTextureStorage3D(textureID, descriptor.mipLevels, textureFormat, descriptor.width, descriptor.height, descriptor.arrayLayers);
				break;
		}

		Texture texture;
		{
			texture.handle = textureID;
			texture.descriptor = descriptor;
		}

		return texture;
	}
}