#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <sol/sol.hpp>
#include <json.hpp>

#include "core/defines.hpp"

namespace glm
{
	using vec2u8 = glm::vec<2, tur::u8, glm::defaultp>;
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(vec2u8, x, y);

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(vec2, x, y);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(uvec2, x, y);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(vec3, x, y, z);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(vec4, x, y, z, w);
}


