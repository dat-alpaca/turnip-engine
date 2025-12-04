#pragma once
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
		b2BodyDef bodyDef;
		b2BodyId bodyID;
	};

	struct RectCollider2D
	{
	public:
		RectCollider2D(float width, float height)
		{
			polygon = b2MakeBox(width / 2.f, height / 2.f);
			shape = b2DefaultShapeDef();
			shape.density = 1.0f;
		}

	public:
		b2Polygon polygon;
		b2ShapeDef shape;
	};
}