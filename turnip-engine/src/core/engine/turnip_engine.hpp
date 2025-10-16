#pragma once
#include "assets/asset_library.hpp"
#include "graphics/asset_texture_mapper.hpp"
#include "platform/platform.hpp"
#include "rhi/rhi.hpp"
#include "scene/scene_holder.hpp"
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
		SceneHolder& get_scene_holder() { return mSceneHolder; }
		ViewSystem& get_view_system() { return mViewSystem; }
		WorkerPool& get_worker_pool() { return mWorkerPool; }
		AssetLibrary& get_asset_library() { return mAssetLibrary; }

	public:
		AssetTextureMapper& get_asset_texture_mapper() { return mAssetTextureMapper; }

	private:
		RenderInterface mRenderInterface;
		SceneHolder mSceneHolder;
		ViewSystem mViewSystem;
		WorkerPool mWorkerPool;
		tur_unique<Window> mWindow;
		AssetLibrary mAssetLibrary;

	private:
		AssetTextureMapper mAssetTextureMapper;

	private:
		bool mShutdownRequested = false;
	};
}