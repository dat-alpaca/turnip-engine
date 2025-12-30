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
		void set_type(BodyType type)
		{
			bodyDef.type = static_cast<b2BodyType>(type);
			this->type = type;
		}

	public:
		BodyType type = BodyType::STATIC;
		b2BodyDef bodyDef;
		b2BodyId bodyID;
	};

	struct RectCollider2D
	{
	public:
		RectCollider2D() = default;

	public:
		float width = 1.0f, height = 1.0f;

		b2Polygon polygon;
		b2ShapeDef shapeDef;
		b2ShapeId shapeID;
	};
}