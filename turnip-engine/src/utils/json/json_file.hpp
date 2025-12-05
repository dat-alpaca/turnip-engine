#pragma once
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <optional>

#include "core/logging/logging.hpp"

namespace tur
{
	inline nlohmann::json read_json(const std::filesystem::path& filepath)
	{
		using namespace nlohmann;

		std::ifstream file(filepath);
		return nlohmann::json::parse(file);
	}

	template <typename DataType>
	inline std::optional<DataType> json_parse_file(const std::filesystem::path& filepath)
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

		DataType result;
		try
		{
			result = jsonObject.template get<DataType>();
		}
		catch (nlohmann::json::exception& e)
		{
			TUR_LOG_ERROR("Failed to get element in json file at: {}. {}", filepath.string(), e.what());
			return std::nullopt;
		}

		return result;
	}

	inline std::optional<nlohmann::json> json_parse_file(const std::filesystem::path& filepath)
	{
		return json_parse_file<nlohmann::json>(filepath);
	}

	template <typename DataType>
	inline void json_write_file(const std::filesystem::path& filepath, const DataType& dataType)
	{
		nlohmann::json json{dataType};

		std::ofstream jsonFile(filepath);
		jsonFile << std::setfill(' ') << std::setw(4) << json[0];
	}

	inline void json_write_file(const std::filesystem::path& filepath, nlohmann::json jsonObject)
	{
		TUR_LOG_INFO("{}", filepath.string());
		std::ofstream jsonFile(filepath);
		jsonFile << std::setfill(' ') << std::setw(4) << jsonObject;
	}
}