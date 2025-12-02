#pragma once
#include <box2d/box2d.h>

namespace tur
{
	enum class BodyType
	{
		STATIC = b2_staticBody,
		KINEMATIC = b2_kinematicBody,
		DYNAMIC = b2_dynamicBody
	};
}