#pragma once
#include "common.hpp"
#include "graphics/objects/texture.hpp"

namespace tur
{
	struct DefaultTexture
	{
		constexpr static inline u8 Data[] = {
			210, 210, 210, 255, 240, 240, 240, 255, 240, 240, 240, 255, 210, 210, 210, 255,
		};

		constexpr static inline u32 Width = 2;
		constexpr static inline u32 Height = 2;
		constexpr static inline u32 Channels = 4;
	};
}