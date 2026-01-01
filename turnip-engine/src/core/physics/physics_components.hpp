#pragma once
#include "box2d/id.h"
#include "physics_types.hpp"
#include <box2d/box2d.h>
#include <glm/glm.hpp>

namespace tur
{
	struct Body2DComponent
	{
	public:
		Body2DComponent() = default;

	public:
		BodyType type = BodyType::STATIC;
		b2BodyDef bodyDef;
		b2BodyId bodyID;
		bool isBullet = false;
	};

	struct RectCollider2D
	{
	public:
		RectCollider2D() = default;

	public:
		float width = 1.0f, height = 1.0f;
		float density = 1.0f;
		float friction = 1.0f;
		float restitution = 1.0f;
		float rollingResistance = 1.0f;

		b2Polygon polygon;
		b2ShapeDef shapeDef;
		b2ShapeId shapeID;
	};
}