#pragma once
#include "assets/asset_library.hpp"
#include "audio/audio_handler.hpp"
#include "config/config_data.hpp"
#include "physics/physics_handler.hpp"
#include "script/script_handler.hpp"
#include "view/view.hpp"
#include "worker/worker_pool.hpp"
#include "utils/time/time.hpp"

#include "platform/platform.hpp"
#include "rhi/rhi.hpp"
#include <filesystem>

namespace tur
{
	class TurnipEngine
	{
	public:
		TurnipEngine(const std::filesystem::path& configFilepath);

	public:
		void start();
		void shutdown();

	public:
		view_handle add_view(tur_unique<View> view);
		void remove_view(view_handle viewHandle);

	public:
		void load_scene(const std::filesystem::path& filepath, const Project& project, Scene* scene);

	public:
		RenderInterface& get_render_interface() { return mRenderInterface; }
		ViewHandler& get_view_handler() { return mViewHandler; }
		WorkerPool& get_worker_pool() { return mWorkerPool; }
		PhysicsHandler& get_physics_handler() { return mPhysicsHandler; }
		AssetLibrary& get_asset_library() { return mAssetLibrary; }
		ScriptHandler& get_script_handler() { return mScriptHandler; }

	public:
		glm::vec2 get_window_dimensions() const { return mWindow->get_dimensions(); }

	public:
		const EventCallback& get_event_callback() const { return mMainEventCallback; }

	private:
		RenderInterface mRenderInterface;
		ScriptHandler mScriptHandler;
		ViewHandler mViewHandler;
		WorkerPool mWorkerPool;
		AudioHandler mAudioHandler;
		PhysicsHandler mPhysicsHandler;
		tur_unique<Window> mWindow;

	private:
		AssetLibrary mAssetLibrary;

	private:
		ConfigData mConfigData;
		Time mTime;
		
		f64 mFixedUpdateTimer = 0.0f;

		EventCallback mMainEventCallback;
		bool mShutdownRequested = false;
	};
}