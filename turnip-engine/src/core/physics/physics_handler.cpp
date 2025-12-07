#include "physics_handler.hpp"
#include "box2d/types.h"
#include "logging/logging.hpp"

namespace tur
{
	void PhysicsHandler::initialize(const PhysicsConfig& config)
	{
		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.gravity = (b2Vec2){0.0f, config.gravityAcceleration};

		mWorldId = b2CreateWorld(&worldDef);

		mFixedTimeStep = config.fixedTimeStep;
		mSubstepCount = config.substepCount;
	}
	void PhysicsHandler::shutdown()
	{
		b2DestroyWorld(mWorldId);
		mWorldId = b2_nullWorldId;
	}

	void PhysicsHandler::step()
	{
		b2World_Step(mWorldId, mFixedTimeStep, mSubstepCount);
	}

	b2ContactEvents PhysicsHandler::get_contact_events()
	{
		return b2World_GetContactEvents(mWorldId);
	}
}
