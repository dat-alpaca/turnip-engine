#include "physics_handler.hpp"

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
	}

	void PhysicsHandler::step()
	{
		b2World_Step(mWorldId, mFixedTimeStep, mSubstepCount);
	}
}
