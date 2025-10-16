#pragma once
#include "common.hpp"
#include <filesystem>

#include "asset.hpp"
#include "core/event/events.hpp"
#include "core/worker/worker_pool.hpp"
#include "data-structures/free_list.hpp"
#include "texture_asset.hpp"

namespace tur
{
	class AssetLibrary
	{
	public:
		void initialize(WorkerPool* workerPool);
		void set_event_callback(EventCallback&& eventCallback);

	public:
		asset_handle load_texture(const std::filesystem::path& filepath);
		void unload_texture(asset_handle handle);
		TextureAsset get_texture(asset_handle handle);

	private:
		asset_handle update_texture_data(const std::filesystem::path& filepath, asset_handle target);

	private:
		WorkerPool* rWorkerPool = nullptr;
		EventCallback mEventCallback;

		free_list<TextureAsset> mTextures;
		std::unordered_map<std::string, asset_handle> mAssetFilepathMap;
	};
}
