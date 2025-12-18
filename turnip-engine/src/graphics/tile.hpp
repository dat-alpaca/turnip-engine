#pragma once
#include <glm/glm.hpp>
#include "common.hpp"

namespace tur
{
    struct Tile
	{
	public:
		glm::vec<2, u8, glm::defaultp> position;
		u8 layer;
		u8 flags;
	};

	struct TilemapChunk
	{
		std::vector<Tile> chunks;
	};
}