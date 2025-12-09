#pragma once
#include "common.hpp"
#include "physics/physics_handler.hpp"
#include "platform/platform.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"

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
		void initialize();
		void initialize_input(Window& window);
		void initialize_physics(NON_OWNING PhysicsHandler* physicsHandler);
		void initialize_audio(NON_OWNING AudioHandler* audioHandler);

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
		sol::state mLua;
	};
}