#pragma once
#include <filesystem>
#include <sol/sol.hpp>

#include "common.hpp"

namespace tur
{
	struct Script
	{
	public:
		void start(sol::state& lua)
		{
			sol::protected_function_result result = lua.safe_script_file(filepath, environment);

			if (!result.valid())
			{
				sol::error err = result;
				TUR_LOG_ERROR("[Lua]: {}", err.what());
			}
		}

		void call(std::string_view functionName)
		{
			auto function = environment[functionName];
			if (!function.valid() || function.get_type() != sol::type::function)
				TUR_LOG_ERROR("Failed to call function: {}", functionName);

			function();
		}

	public:
		std::filesystem::path filepath;
		sol::environment environment;
	};
}