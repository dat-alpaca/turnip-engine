#pragma once
#include "defines.hpp"
#include "event/events.hpp"
#include "worker/worker_pool.hpp"
#include "data-structures/free_list.hpp"

#include "assets/asset.hpp"
#include "audio/audio_handler.hpp"
#include "texture/texture_options.hpp"
#include "assets/mesh/mesh_asset.hpp"

#include <filesystem>

namespace tur
{
	class AssetLibrary
	{
	public:
		static inline asset_handle DefaultTextureHandle = 0; 

	public:
		void initialize(WorkerPool* workerPool, AudioHandler* audioHandler);
		void shutdown();

		void set_event_callback(const EventCallback& eventCallback);
		void create_default_texture();

	public:
		asset_handle load_texture_async(const std::filesystem::path& filepath, const TextureOptions& options = {});
		asset_handle load_audio_async(const std::filesystem::path& filepath);
		asset_handle load_mesh_async(const std::filesystem::path& filepath);

	public:
		inline asset_handle get_asset_handle(const std::filesystem::path& filepath)
		{
			if(mFilepathCache.find(filepath) == mFilepathCache.end())
				return invalid_handle;

			return mFilepathCache.at(filepath);
		}

		inline TextureAsset& get_texture_asset(asset_handle handle)
		{
			if (handle == invalid_handle)
				TUR_LOG_CRITICAL("Invalid asset handle passed to get_texture_asset()");

			return mTextureAssets.get(handle);
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

	private:
		NON_OWNING WorkerPool* rWorkerPool = nullptr;
		NON_OWNING AudioHandler* rAudioHandler = nullptr;
		EventCallback mEventCallback;

	private:
		std::unordered_map<std::filesystem::path, asset_handle> mFilepathCache;
		free_list<TextureAsset> mTextureAssets;
		free_list<AudioAsset> mAudioAssets;
		free_list<MeshAsset> mMeshAssets;
	};
}