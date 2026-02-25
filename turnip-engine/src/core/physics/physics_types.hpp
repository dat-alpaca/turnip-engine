#pragma once
#include <box2d/box2d.h>
#include "utils/string_utils.hpp"

namespace tur
{
	enum class BodyType
	{
		STATIC = b2_staticBody,
		KINEMATIC = b2_kinematicBody,
		DYNAMIC = b2_dynamicBody
	};

	static inline BodyType get_body_type(const std::string& name)
	{
		std::string converted = name;
		to_upper(converted);

        if (converted == "STATIC")
			return BodyType::STATIC;

        if (converted == "KINEMATIC")
			return BodyType::KINEMATIC;

        if (converted == "DYNAMIC")
			return BodyType::DYNAMIC;

		return BodyType::STATIC;
	}

	static inline constexpr const char* get_body_type_name(BodyType type)
	{
		if (type == BodyType::STATIC)
			return "STATIC";

		if (type == BodyType::KINEMATIC)
			return "KINEMATIC";

		if (type == BodyType::DYNAMIC)
			return "DYNAMIC";

		return "INVALID";
	}
}