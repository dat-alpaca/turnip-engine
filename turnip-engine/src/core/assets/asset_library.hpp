#pragma once
#include "data-structures/free_list.hpp"
#include "event/events.hpp"
#include "worker/worker_pool.hpp"

#include "audio/audio_handler.hpp"
#include "audio/audio_loader.hpp"
#include "texture/texture_loader.hpp"
#include "texture/texture_options.hpp"

#include <filesystem>

namespace tur
{
	class AssetLibrary
	{
	public:
		void initialize(WorkerPool* workerPool, AudioHandler* audioHandler);
		void shutdown();

		void set_event_callback(const EventCallback& eventCallback);

	public:
		asset_handle load_texture_async(const std::filesystem::path& filepath, const TextureOptions& options = {});
		asset_handle load_audio_async(const std::filesystem::path& filepath);

	public:
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

	private:
		NON_OWNING WorkerPool* rWorkerPool = nullptr;
		NON_OWNING AudioHandler* rAudioHandler = nullptr;
		EventCallback mEventCallback;

	private:
		std::unordered_map<std::filesystem::path, asset_handle> mFilepathCache;
		free_list<TextureAsset> mTextureAssets;
		free_list<AudioAsset> mAudioAssets;
	};
}