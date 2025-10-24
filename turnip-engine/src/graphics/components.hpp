#pragma once
#include "graphics/objects/texture.hpp"

namespace tur
{
	struct Sprite2DComponent
	{
	public:
		Sprite2DComponent() = default;
		Sprite2DComponent(texture_handle textureHandle)
			: textureHandle(textureHandle)
		{
		}

	public:
		texture_handle textureHandle = invalid_handle;
	};
}