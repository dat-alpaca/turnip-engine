#include "physics_handler.hpp"
#include "box2d/box2d.h"
#include "box2d/id.h"
#include "box2d/types.h"

namespace tur
{
	void PhysicsHandler::initialize(const PhysicsConfig& config)
	{
		mFixedTimeStep = config.fixedTimeStep;
		mSubstepCount = config.substepCount;

		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.gravity = (b2Vec2){0.0f, config.gravityAcceleration};
		
		mWorld = b2CreateWorld(&worldDef);
	}
	void PhysicsHandler::shutdown()
	{
		b2DestroyWorld(mWorld);
		mWorld = b2_nullWorldId;
	}

	void PhysicsHandler::step()
	{
		b2World_Step(mWorld, mFixedTimeStep, mSubstepCount);
	}

	b2ContactEvents PhysicsHandler::get_contact_events()
	{
		return b2World_GetContactEvents(mWorld);
	}
}
