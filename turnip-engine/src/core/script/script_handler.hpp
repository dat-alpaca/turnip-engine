#pragma once
#include "common.hpp"
#include "memory/memory.hpp"
#include "physics/physics_handler.hpp"
#include "platform/platform.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"
#include "scene/scene_holder.hpp"

#include <filesystem>
#include <sol/sol.hpp>
#include <string_view>
#include <vector>

namespace tur
{
	class AudioHandler;
	class PhysicsHandler;

	class ScriptHandler
	{
	public:
		void initialize(NON_OWNING Window* window);
		void initialize_input();
		void initialize_physics(NON_OWNING PhysicsHandler* physicsHandler);
		void initialize_audio(NON_OWNING AudioHandler* audioHandler);
		void initialize_scene(NON_OWNING SceneHolder* sceneHolder);

	public:
		sol::object find_component(Entity entity, const std::string& componentName);
		sol::object add_component(Entity entity, const std::string& componentName);

	private:
		void initialize_logging();
		void initialize_usertypes();

	public:
		sol::state& get_state() { return mLua; }

	private:
		NON_OWNING AudioHandler* rAudioHandler = nullptr;
		NON_OWNING Window* rWindow = nullptr;
		NON_OWNING SceneHolder* rSceneHolder = nullptr;
		sol::state mLua;
	};
}