#pragma once
#include "config/physics_config.hpp"
#include <box2d/box2d.h>

namespace tur
{
	class PhysicsHandler
	{
	public:
		void initialize(const PhysicsConfig& config);
		void shutdown();

	public:
		void step();

	public:
		b2WorldId get_world_id() const { return mWorldId; }

	private:
		b2WorldId mWorldId;
		u32 mSubstepCount;
		float mFixedTimeStep;
	};
}