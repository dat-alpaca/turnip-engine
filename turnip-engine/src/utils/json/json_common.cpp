#include "json_common.hpp"
#include "utils/uuid/uuid.hpp"

namespace glm
{
	void to_json(nlohmann::json& json, const glm::vec<2, u8, glm::defaultp>& vector)
	{
		json = {{"x", vector.x}, {"y", vector.y}};
	}
	void from_json(const nlohmann::json& json, glm::vec<2, u8, glm::defaultp>& vector)
	{
		vector.x = json.at("x").get<u32>();
		vector.y = json.at("y").get<u32>();
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

	void to_json(nlohmann::json& json, const glm::vec2& vector)
	{
		json = {{"x", vector.x}, {"y", vector.y}};
	}
	void from_json(const nlohmann::json& json, glm::vec2& vector)
	{
		vector.x = json.at("x").get<float>();
		vector.y = json.at("y").get<float>();
	}

	void to_json(nlohmann::json& json, const glm::vec3& vector)
	{
		json = {{"x", vector.x}, {"y", vector.y}, {"z", vector.z}};
	}
	void from_json(const nlohmann::json& json, glm::vec3& vector)
	{
		vector.x = json.at("x").get<float>();
		vector.y = json.at("y").get<float>();
		vector.z = json.at("z").get<float>();
	}

	void to_json(nlohmann::json& json, const glm::vec4& vector)
	{
		json = {{"x", vector.x}, {"y", vector.y}, {"z", vector.z}, {"w", vector.w}};
	}
	void from_json(const nlohmann::json& json, glm::vec4& vector)
	{
		vector.x = json.at("x").get<float>();
		vector.y = json.at("y").get<float>();
		vector.z = json.at("z").get<float>();
		vector.w = json.at("w").get<float>();
	}
}

namespace sol
{
	static nlohmann::json table_to_json(const sol::table& table)
	{
		nlohmann::json object;

		for (auto& [key, value] : table.pairs())
		{
			if (key.is<std::string>()) 
			{
				std::string keyString = key.as<std::string>();

				if (value.is<sol::table>()) 
					object[keyString] = table_to_json(value.as<sol::table>());

				else if (value.is<bool>()) 
					object[keyString] = value.as<bool>();

				else if (value.is<int>()) 
					object[keyString] = value.as<int>();

				else if (value.is<double>()) 
					object[keyString] = value.as<double>();

				else if (value.is<std::string>()) 
					object[keyString] = value.as<std::string>();

				else 
					object[keyString] = nullptr;
			}

			else if (key.is<int>())
			{
				int index = key.as<int>() - 1;

				if (object.is_null()) 
					object = nlohmann::json::array();

				if (value.is<sol::table>()) 
					object[index] = table_to_json(value.as<sol::table>());

				else if (value.is<bool>()) 
					object[index] = value.as<bool>();

				else if (value.is<int>()) 
					object[index] = value.as<int>();

				else if (value.is<double>()) 
					object[index] = value.as<double>();

				else if (value.is<std::string>()) 
					object[index] = value.as<std::string>();

				else 
					object[index] = nullptr;
			}
		}

		return object;
	}

	static sol::table json_to_table(sol::state_view lua, const nlohmann::json& object) 
	{
		sol::table table = lua.create_table();

		if (object.is_object()) 
		{
			for (auto& [key, value] : object.items()) 
			{
				if (value.is_object() || value.is_array())
					table[key] = json_to_table(lua, value);

				else if (value.is_boolean())
					table[key] = value.get<bool>();

				else if (value.is_number_integer())
					table[key] = value.get<int>();
				
				else if (value.is_number_float())
					table[key] = value.get<double>();

				else if (value.is_string())
					table[key] = value.get<std::string>();

				else
					table[key] = sol::nil;
			}
		}

		else if (object.is_array()) 
		{
			int i = 1;
			for (auto& value : object) 
			{
				if (value.is_object() || value.is_array())
					table[i] = json_to_table(lua, value);

				else if (value.is_boolean())
					table[i] = value.get<bool>();

				else if (value.is_number_integer())
					table[i] = value.get<int>();

				else if (value.is_number_float())
					table[i] = value.get<double>();

				else if (value.is_string())
					table[i] = value.get<std::string>();

				else
					table[i] = sol::nil;

				++i;
			}
		}

		return table;
	}
	
	void to_json(nlohmann::json& json, const sol::table& table)
	{
		json = table_to_json(table);
	}
	void from_json(const nlohmann::json& json, sol::table& table)
	{
		sol::state_view lua(table.lua_state());
    	table = json_to_table(lua, json);
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