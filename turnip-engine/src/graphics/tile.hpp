#pragma once
#include <glm/glm.hpp>
#include "common.hpp"

namespace tur
{
	struct TileFlags
	{
	public:
		void set_enable(bool enable)
		{
			this->enable = static_cast<u8>(enable); 
		}

		void set_flip(bool flip)
		{
			this->flip = static_cast<u8>(flip); 
		}

		void set_animation_size(u8 animationSize)
		{
			this->animationSize = animationSize & 0xFC; 
		}

	public:
		u8 enable : 1;
		u8 flip : 1;
		u8 animationSize : 6;
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