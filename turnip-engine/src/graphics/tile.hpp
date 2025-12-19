#pragma once
#include <glm/glm.hpp>
#include "common.hpp"
#include "glm/detail/qualifier.hpp"

namespace tur
{
	struct TileFlags
	{
	public:
		void set_enable(bool enable)
		{
			if (enable)
            	data |= 0b00000001;
			else
            	data &= 0b11111110;
		}

		void set_flip(bool flip)
		{
			if (flip)
            	data |= 0b00000010;
			else
            	data &= 0b11111101;
		}

		void set_animation_size(u8 animationSize)
		{
			data = (data & 0b00000011) | ((animationSize & 0b00111111) << 2);
		}

		u8 value() const
		{
			return data;
		}

	public:
		u8 data = 0b00000001;
	};

	struct Tile
	{
	public:
		glm::vec<2, u8, glm::defaultp> position;
		u8 layer;
		TileFlags flags;
	};

	struct TilemapChunk
	{
		std::vector<Tile> chunks;
	};
}