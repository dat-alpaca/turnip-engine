#include "json_common.hpp"
#include "utils/uuid/uuid.hpp"

namespace glm
{
	void to_json(nlohmann::json& json, const glm::vec2& vector)
	{
		json = {{"x", vector.x}, {"y", vector.y}};
	}
	void from_json(const nlohmann::json& json, glm::vec2& vector)
	{
		vector.x = json.at("x").get<float>();
		vector.y = json.at("y").get<float>();
	}

	void to_json(nlohmann::json& json, const glm::uvec2& vector)
	{
		json = {{"x", vector.x}, {"y", vector.y}};
	}
	void from_json(const nlohmann::json& json, glm::uvec2& vector)
	{
		vector.x = json.at("x").get<u32>();
		vector.y = json.at("y").get<u32>();
	}
}

namespace tur
{
	void to_json(nlohmann::json& json, const UUID& uuid)
	{
		json = uuid.uuid;
	}
	void from_json(const nlohmann::json& json, UUID& uuid)
	{
		uuid.uuid = json;
	}
}