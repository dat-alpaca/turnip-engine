#pragma once
#include <glm/glm.hpp>

namespace tur
{
	struct Body2D
	{
	public:
		Body2D() {}

	public:
		glm::vec2 position;
		bool isDynamic;
	};

	struct Collider2D
	{
	};
}