#pragma once
#include <sol/sol.hpp>
#include <string_view>
#include "common.hpp"

namespace tur
{
	template<typename... Args>
	inline void call_expected_function(sol::table& instance, std::string_view functionName, Args&&... args)
	{
		auto function = instance[functionName];

		if (!function.valid() || function.get_type() != sol::type::function)
			TUR_LOG_ERROR("Failed to call function: {}", functionName);

		function(instance, args...);
	}

	inline bool has_function(sol::table& instance, std::string_view functionName)
	{
		if(instance == sol::nil)
			return false;

		return instance[functionName].valid();
	}

	inline sol::function get_function(sol::table& instance, std::string_view functionName)
	{
		auto function = instance[functionName];
		if (!function.valid() || function.get_type() != sol::type::function)
			return {};

		return function;
	}
}