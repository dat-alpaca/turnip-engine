#pragma once
#include "assets/asset_library.hpp"
#include "assets/texture_asset_binder.hpp"
#include "platform/platform.hpp"
#include "rhi/rhi.hpp"
#include "script/script_system.hpp"
#include "view/view.hpp"
#include "worker/worker_pool.hpp"

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
		ViewSystem& get_view_system() { return mViewSystem; }
		WorkerPool& get_worker_pool() { return mWorkerPool; }
		AssetLibrary& get_asset_library() { return mAssetLibrary; }

	public:
		TextureAssetBinder& get_texture_asset_binder() { return mTextureAssetBinder; }
		const EventCallback& get_event_callback() const { return mMainEventCallback; }

	private:
		RenderInterface mRenderInterface;
		ViewSystem mViewSystem;
		WorkerPool mWorkerPool;
		tur_unique<Window> mWindow;

	private:
		AssetLibrary mAssetLibrary;
		TextureAssetBinder mTextureAssetBinder;

	private:
		EventCallback mMainEventCallback;
		bool mShutdownRequested = false;
	};
}