#pragma once
#include <json.hpp>
#include <fstream>
#include <optional>

#include "common.hpp"

namespace tur
{
    inline nlohmann::json read_json(const std::filesystem::path& filepath)
	{
		std::ifstream file(filepath);
		return nlohmann::json::parse(file);
	}

	template <typename T>
	inline std::optional<T> parse_json(const std::filesystem::path& filepath)
	{
		nlohmann::json jsonObject;
		try
		{
			jsonObject = read_json(filepath);
		}
		catch (nlohmann::json::exception& e)
		{
			TUR_LOG_ERROR("Failed to parse json file at: {}. {}", filepath.string(), e.what());
			return std::nullopt;
		}

		T result;
		try
		{
			result = jsonObject.get<T>();
		}
		catch (nlohmann::json::exception& e)
		{
			TUR_LOG_ERROR("Failed to get element in json file at: {}. {}", filepath.string(), e.what());
			return std::nullopt;
		}

		return result;
	}

	inline void write_json(const std::filesystem::path& filepath, const nlohmann::json& data)
	{
		std::ofstream file(filepath);
		file
            << std::setfill(' ') 
            << std::setw(4)
            << std::setprecision(17)
            << data;
	}
}