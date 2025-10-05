#pragma once
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

#include "utils/json/json_common.hpp"
#include "utils/string_utils.hpp"
#include "windowing/windowing.hpp"

namespace tur
{
	inline void to_json(nlohmann::json& json, const WindowingCapabilities& capabilities)
	{
		json["api"] = get_graphics_api_name(capabilities.api);
		json["major"] = capabilities.major;
		json["minor"] = capabilities.minor;
		json["patch"] = capabilities.patch;
		json["variant"] = capabilities.variant;
	}

	inline void from_json(const nlohmann::json& json, WindowingCapabilities& capabilities)
	{
		capabilities.api = get_graphics_api_from_string(json.at("api"));
		capabilities.major = json.at("major");
		capabilities.minor = json.at("minor");
		capabilities.patch = json.at("patch");
		capabilities.variant = json.at("variant");
	}

	inline void to_json(nlohmann::json& json, const WindowProperties& properties)
	{
		json["title"] = properties.title;
		json["dimensions"] = properties.dimensions;
		json["minSize"] = properties.minSize;
		json["maxSize"] = properties.maxSize;
		json["startsMaximized"] = properties.startsMaximized;

		if (properties.position == invalid_size)
		{
			json["position"] = {{"x", "default"}, {"y", "default"}};
		}
	}

	inline void from_json(const nlohmann::json& json, WindowProperties& properties)
	{
		properties.title = json["title"];
		properties.dimensions = json["dimensions"];
		properties.minSize = json["minSize"];
		properties.maxSize = json["maxSize"];
		properties.startsMaximized = json["startsMaximized"];

		auto xPosString = json["position"].at("x").get<std::string>();
		to_lower(xPosString);
		if (xPosString != "default")
			properties.position.x = json["position"].at("x").get<u32>();

		auto yPosString = json["position"].at("y").get<std::string>();
		to_lower(yPosString);
		if (yPosString != "default")
			properties.position.x = json["position"].at("y").get<u32>();
	}
}