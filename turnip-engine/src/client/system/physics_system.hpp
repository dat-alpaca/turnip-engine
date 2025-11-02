#pragma once
#include "physics/physics_handler.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"

namespace tur
{
	class PhysicsSystem
	{
	public:
		void initialize(NON_OWNING Scene* scene, NON_OWNING PhysicsHandler* physicsHandler)
		{
			rPhysicsHandler = physicsHandler;
			set_scene(scene);
		}
		void set_scene(NON_OWNING Scene* scene) { rScene = scene; }

	public:
		void update() {}

	private:
		NON_OWNING PhysicsHandler* rPhysicsHandler = nullptr;
		NON_OWNING Scene* rScene = nullptr;
	};
}