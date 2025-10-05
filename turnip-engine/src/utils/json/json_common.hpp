#pragma once
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

namespace glm
{
	void to_json(nlohmann::json& json, const glm::vec2& vector);
	void from_json(const nlohmann::json& json, glm::vec2& vector);
	void to_json(nlohmann::json& json, const glm::uvec2& vector);
	void from_json(const nlohmann::json& json, glm::uvec2& vector);
}

namespace tur
{
	class UUID;

	void to_json(nlohmann::json& json, const UUID& uuid);
	void from_json(const nlohmann::json& json, UUID& uuid);
}