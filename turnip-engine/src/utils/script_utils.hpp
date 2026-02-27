#pragma once
#include <sol/sol.hpp>
#include <string_view>
#include "common.hpp"
#include "logging/logging.hpp"

namespace tur
{
	template<typename... Args>
	inline void call_expected_function(sol::table& instance, std::string_view functionName, Args&&... args)
	{
		auto functionObject = instance[functionName];

		if (!functionObject.valid() || functionObject.get_type() != sol::type::function)
			TUR_LOG_ERROR("Failed to call function: {}", functionName);

		sol::protected_function function = functionObject;
		sol::protected_function_result result = function(instance, args...);
		
		if(result.valid())
			return;

		sol::error error = result;
		std::string what = error.what();

		TUR_LOG_ERROR("[Script Error]: Function: [{}] | {}", functionName, what);
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