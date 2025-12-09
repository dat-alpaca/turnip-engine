#pragma once
#include "config/physics_config.hpp"
#include "event/event.hpp"
#include "event/events.hpp"

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
		b2ContactEvents get_contact_events();

	public:
		b2WorldId get_world_id() const { return mWorldId; }

	public:
		inline u32 get_substep_count() const { return mSubstepCount; }
		inline float get_fixed_timestep() const { return mFixedTimeStep; }
	
	private:
		b2WorldId mWorldId;
		u32 mSubstepCount;
		float mFixedTimeStep;
	};
}