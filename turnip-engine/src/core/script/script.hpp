#pragma once
#include <filesystem>
#include <sol/sol.hpp>

#include "common.hpp"

namespace tur
{
	struct Script
	{
	public:
		void start(sol::state& lua) { lua.script_file(filepath, environment); }

		void call(std::string_view functionName)
		{
			auto function = environment[functionName];
			if (!function.valid() || function.get_type() != sol::type::function)
				TUR_LOG_CRITICAL("Failed to call function: {}", functionName);

			function();
		}

	public:
		std::filesystem::path filepath;
		sol::environment environment;
	};
}