#pragma once
#include <glm/glm.hpp>

namespace tur
{
    struct MeshVertex
	{
		glm::vec3 position;
		float uvx;

		glm::vec3 normals;
		float uvy;
	};
}