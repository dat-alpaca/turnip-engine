#pragma once
#include <filesystem>
#include <fstream>
#include <portable-file-dialogs.h>
#include <sstream>
#include <string>

#include "common.hpp"
#include "core/logging/logging.hpp"

namespace tur
{
	inline std::string read_file(const std::filesystem::path& filepath)
	{
		std::ifstream file(filepath);
		if (!std::filesystem::exists(filepath))
		{
			TUR_LOG_ERROR("Filepath does not exist: {}", filepath.string().c_str());
			return "";
		}

		if (!file.is_open())
		{
			TUR_LOG_ERROR("Failed to load file: {}", filepath.string().c_str());
			return "";
		}

		std::stringstream ss;
		ss << file.rdbuf();

		return ss.str();
	}

	inline std::vector<u8> read_file_binary(const std::filesystem::path& filepath)
	{
		if (!std::filesystem::exists(filepath))
		{
			TUR_LOG_ERROR("Filepath does not exist: {}", filepath.string().c_str());
			return {};
		}

		std::ifstream file(filepath, std::ios::binary | std::ios::ate);
		if (!file.is_open())
			TUR_LOG_ERROR("Failed to load file: {}", filepath.string().c_str());

		u64 fileSize = static_cast<u64>(file.tellg());
		std::vector<u8> data(fileSize);

		file.seekg(0);
		file.read(reinterpret_cast<char*>(data.data()), fileSize);

		file.close();

		return data;
	}

	inline std::string save_file_dialog(const std::string& title, const std::string& defaultPath = "",
										const std::vector<std::string>& filters = {})
	{
		return pfd::save_file(title, defaultPath, filters).result();
	}

	inline std::vector<std::string>
	open_file_dialog(const std::string& title, const std::vector<std::string>& filters = {}, bool allowMulti = false)
	{
		if (!allowMulti)
			return pfd::open_file(title, "", filters).result();
		else
			return pfd::open_file(title, "", filters, pfd::opt::multiselect).result();
	}

	inline std::string open_folder_dialog(const std::string& title)
	{
		return pfd::select_folder(title, "").result();
	}
}