#pragma once
#include "assets/asset_library.hpp"
#include "audio/audio_handler.hpp"
#include "physics/physics_handler.hpp"
#include "script/script_handler.hpp"
#include "view/view.hpp"
#include "worker/worker_pool.hpp"

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
		void add_view(tur_unique<View> view);
		void remove_view(view_handle viewHandle);

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
		ViewHandler mViewHandler;
		WorkerPool mWorkerPool;
		AudioHandler mAudioHandler;
		PhysicsHandler mPhysicsHandler;
		ScriptHandler mScriptHandler;
		tur_unique<Window> mWindow;

	private:
		AssetLibrary mAssetLibrary;

	private:
		EventCallback mMainEventCallback;
		bool mShutdownRequested = false;
	};
}