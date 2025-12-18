#pragma once
#include <glm/glm.hpp>
#include "common.hpp"

namespace tur
{
    struct Tile
	{
	public:
		glm::uvec2 position;
		u32 layer;
		u32 flags;
	};

	struct TilemapChunk
	{
		std::vector<Tile> chunks;
	};
}