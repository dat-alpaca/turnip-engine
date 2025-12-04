#pragma once
#include "common.hpp"
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

	class ScriptHandler
	{
	public:
		void initialize();
		void initialize_input(Window& window);
		void initialize_audio(NON_OWNING AudioHandler* audioHandler);

	private:
		void initialize_logging();
		void initialize_usertypes();

	public:
		inline sol::environment create_environment() { return sol::environment(mLua, sol::create, mLua.globals()); }

	public:
		sol::state& get_state() { return mLua; }

	private:
		NON_OWNING AudioHandler* rAudioHandler = nullptr;
		sol::state mLua;
	};
}