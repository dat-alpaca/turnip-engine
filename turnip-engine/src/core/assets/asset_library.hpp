#pragma once
#include "defines.hpp"
#include "event/events.hpp"
#include "utils/uuid/uuid.hpp"
#include "worker/worker_pool.hpp"
#include "data-structures/free_list.hpp"

#include "assets/asset.hpp"
#include "assets/mesh/mesh_asset.hpp"
#include "assets/cubemap/cubemap_asset.hpp"

#include "audio/audio_handler.hpp"
#include "texture/texture_options.hpp"

#include <filesystem>
#include <vector>

namespace tur
{
	class AssetLibrary
	{
	public:
		static inline asset_handle DefaultTextureHandle = 0; 

	public:
		void initialize(WorkerPool* workerPool, AudioHandler* audioHandler);
		void on_event(Event& event);
		void shutdown();

		void set_event_callback(const EventCallback& eventCallback);
		void create_default_texture();

	public:
		asset_handle load_texture_async(const std::filesystem::path& filepath, const AssetMetadata& metadata, const TextureOptions& options = {});
		asset_handle load_audio_async(const std::filesystem::path& filepath, const AssetMetadata& metadata);
		asset_handle load_mesh_async(const std::filesystem::path& filepath, const AssetMetadata& metadata);
		asset_handle load_cubemap_async(const std::array<std::filesystem::path, 6>& filepaths, const CubemapAsset& asset);

	public:
		inline asset_handle get_asset_handle(const std::filesystem::path& filepath)
		{
			if(mFilepathCache.find(filepath) == mFilepathCache.end())
				return invalid_handle;

			return mFilepathCache.at(filepath);
		}
		inline asset_handle get_asset_handle(UUID uuid)
		{
			if(mUUIDCache.find(uuid) == mUUIDCache.end())
				return invalid_handle;

			return mUUIDCache.at(uuid);
		}

		inline TextureAsset& get_texture_asset(asset_handle handle)
		{
			if (handle == invalid_handle)
				TUR_LOG_CRITICAL("Invalid asset handle passed to get_texture_asset()");

			return mTextureAssets.get(handle);
		}
		inline CubemapAsset& get_cubemap_asset(asset_handle handle)
		{
			if (handle == invalid_handle)
				TUR_LOG_CRITICAL("Invalid asset handle passed to get_cubemap_asset()");

			return mCubemapAssets.get(handle);
		}
		inline AudioAsset& get_audio_asset(asset_handle handle)
		{
			if (handle == invalid_handle)
				TUR_LOG_CRITICAL("Invalid asset handle passed to get_audio_asset()");

			return mAudioAssets.get(handle);
		}
		inline MeshAsset& get_mesh_asset(asset_handle handle)
		{
			if (handle == invalid_handle)
				TUR_LOG_CRITICAL("Invalid asset handle passed to get_mesh_asset()");

			return mMeshAssets.get(handle);
		}

	public:
		inline free_list<TextureAsset>& get_texture_assets() { return mTextureAssets; };
		inline free_list<AudioAsset>& get_audio_assets() { return mAudioAssets; }
		inline free_list<MeshAsset>& get_mesh_assets() { return mMeshAssets; }
		inline free_list<CubemapAsset>& get_cubemap_assets() { return mCubemapAssets; }

	private:
		NON_OWNING WorkerPool* rWorkerPool = nullptr;
		NON_OWNING AudioHandler* rAudioHandler = nullptr;
		EventCallback mEventCallback;

	private:
		std::unordered_map<std::filesystem::path, asset_handle> mFilepathCache;
		std::unordered_map<UUID, asset_handle> mUUIDCache;

		free_list<TextureAsset> mTextureAssets;
		free_list<AudioAsset> mAudioAssets;
		free_list<MeshAsset> mMeshAssets;
		free_list<CubemapAsset> mCubemapAssets;

		std::vector<asset_handle> mTextureRemoveOnSceneChange;
		std::vector<asset_handle> mAudioRemoveOnSceneChange;
		std::vector<asset_handle> mMeshRemoveOnSceneChange;
		std::vector<asset_handle> mCubemapRemoveOnSceneChange;
	};
}