#pragma once
#include "common.hpp"
#include <filesystem>

#include "asset.hpp"
#include "core/event/events.hpp"
#include "core/worker/worker_pool.hpp"
#include "data-structures/free_list.hpp"
#include "texture_asset.hpp"
#include "texture_asset_binder.hpp"

namespace tur
{
	class AssetLibrary
	{
	public:
		void initialize(WorkerPool* workerPool, TextureAssetBinder* binder);
		void set_event_callback(EventCallback&& eventCallback);

	public:
		asset_handle load_texture(const std::filesystem::path& filepath);
		void load_texture_imm(const std::filesystem::path& filepath, texture_handle& target);
		void unload_texture(asset_handle handle);
		TextureAsset get_texture(asset_handle handle);

	private:
		asset_handle update_texture_data(const std::filesystem::path& filepath, asset_handle target);

	private:
		NON_OWNING TextureAssetBinder* rBinder = nullptr;
		NON_OWNING WorkerPool* rWorkerPool = nullptr;
		EventCallback mEventCallback;

		free_list<TextureAsset> mTextures;
		std::unordered_map<std::string, asset_handle> mAssetFilepathMap;
	};
}
