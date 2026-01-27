#pragma once
#include <box2d/box2d.h>
#include "box2d/collision.h"
#include "box2d/id.h"
#include "box2d/types.h"
#include "config/physics_config.hpp"
#include "defines.hpp"

namespace tur
{
	using world_handle = handle_type;

	class PhysicsHandler
	{
	public:
		void initialize(const PhysicsConfig& config);
		void shutdown();

	public:
		void step();
		b2ContactEvents get_contact_events();

	public:
		b2BodyId create_body(b2BodyDef* bodyDef)
		{
			return b2CreateBody(mWorld, bodyDef);
		}
		void destroy_body(b2BodyId& bodyID)
		{
			b2DestroyBody(bodyID);
			bodyID = b2_nullBodyId;
		}

		b2ShapeId create_shape(b2BodyId bodyID, b2ShapeDef* shapeDef, b2Polygon* polygon)
		{
			return b2CreatePolygonShape(bodyID, shapeDef, polygon);
		}

	public:
		inline u32 get_substep_count() const { return mSubstepCount; }
		inline float get_fixed_timestep() const { return mFixedTimeStep; }
	
	private:
		b2WorldId mWorld;
		u32 mSubstepCount;
		float mFixedTimeStep;
	};
}