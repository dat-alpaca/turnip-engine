#pragma once
#include "box2d/id.h"
#include "defines.hpp"
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
		FIELD_SET_INTERNALLY b2BodyDef bodyDef;
		FIELD_SET_INTERNALLY b2BodyId bodyID = b2_nullBodyId;

		BodyType type = BodyType::STATIC;
		bool isBullet = false;
	};

	struct RectCollider2D
	{
	public:
		RectCollider2D() = default;

	public:
		float width = 1.0f;
		float height = 1.0f;
		float density = 1.0f;
		float friction = 1.0f;
		float restitution = 1.0f;
		float rollingResistance = 1.0f;

		FIELD_SET_INTERNALLY b2Polygon polygon;
		FIELD_SET_INTERNALLY b2ShapeDef shapeDef;
		FIELD_SET_INTERNALLY b2ShapeId shapeID = b2_nullShapeId;
	};
}