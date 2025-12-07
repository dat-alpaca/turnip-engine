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
		Body2DComponent(BodyType type)
		{
			bodyDef = b2DefaultBodyDef();
			set_type(type);
		}

		Body2DComponent()
		{
			bodyDef = b2DefaultBodyDef();
		}

	public:
		void set_type(BodyType type)
		{
			bodyDef.type = static_cast<b2BodyType>(type);
		}

	public:
		// TODO: enableContactEvents
		// TODO: getType
		b2BodyDef bodyDef;
		b2BodyId bodyID;
	};

	struct RectCollider2D
	{
	public:
		RectCollider2D(float width, float height)
		{
			polygon = b2MakeBox(width / 2.f, height / 2.f);
			shapeDef = b2DefaultShapeDef();
			shapeDef.density = 1.0f;
			shapeDef.material.friction = 0.3f; // TODO: rest

			shapeDef.enableContactEvents = true;
			shapeDef.filter.categoryBits = 0x0001;
			shapeDef.filter.maskBits = 0xFFFF;
		}

	public:
		b2Polygon polygon;
		b2ShapeDef shapeDef;
		b2ShapeId shapeID;
	};
}