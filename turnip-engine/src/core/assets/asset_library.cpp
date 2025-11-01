#include "asset_library.hpp"
#include <miniaudio.h>
#include <stb_image.h>

#include "audio/audio_asset.hpp"
#include "texture/texture_loader.hpp"

namespace tur
{
	void AssetLibrary::initialize(WorkerPool* workerPool, AudioHandler* audioHandler)
	{
		TUR_ASSERT(workerPool, "Invalid WorkerPool handle passed");
		TUR_ASSERT(audioHandler, "Invalid AudioHandler handle passed");

		rAudioHandler = audioHandler;
		rWorkerPool = workerPool;
	}
	void AssetLibrary::shutdown()
	{
		mTextureAssets.clear();
		mAudioAssets.clear();
	}

	void AssetLibrary::set_event_callback(const EventCallback& eventCallback)
	{
		mEventCallback = eventCallback;
	}

	asset_handle AssetLibrary::load_texture_async(const std::filesystem::path& filepath, const TextureOptions& options)
	{
		if (mFilepathCache.find(filepath) != mFilepathCache.end())
			return mFilepathCache[filepath];

		TextureAsset loadingAsset = {};
		loadingAsset.state = AssetState::LOADING;
		loadingAsset.options = options;

		asset_handle assetHandle = mTextureAssets.add(loadingAsset);

		rWorkerPool->submit<texture_asset_opt>(
			[filepath]() { return load_texture_task(filepath); },
			[&, filepath, assetHandle, options](const texture_asset_opt& textureAsset)
			{
				if (!textureAsset.has_value())
					return TUR_LOG_ERROR("Failed to load texture asset into the asset library.");

				// TODO: check if float texture and format.
				options.floatTexture;
				options.dataFormat;

				TextureAsset& loadedTextureAsset = mTextureAssets.get(assetHandle);
				loadedTextureAsset = textureAsset.value();
				loadedTextureAsset.state = AssetState::READY;

				mFilepathCache[filepath] = assetHandle;

				AssetLoadedEvent assetLoadedEvent(assetHandle, AssetType::TEXTURE);
				mEventCallback(assetLoadedEvent);
			}
		);

		return assetHandle;
	}

	asset_handle AssetLibrary::load_audio_async(const std::filesystem::path& filepath)
	{
		if (mFilepathCache.find(filepath) != mFilepathCache.end())
			return mFilepathCache[filepath];

		auto& engine = rAudioHandler->get_engine();

		asset_handle assetHandle = mAudioAssets.add({});
		AudioAsset& audioAsset = mAudioAssets.get(assetHandle);
		audioAsset.metadata.filepath = filepath;
		audioAsset.state = AssetState::READY;

		ma_result result = ma_sound_init_from_file(
			&rAudioHandler->mEngine, filepath.string().c_str(), MA_SOUND_FLAG_ASYNC | MA_SOUND_FLAG_NO_SPATIALIZATION,
			nullptr, nullptr, &audioAsset.sound
		);

		if (result != MA_SUCCESS)
		{
			TUR_LOG_ERROR("Failed to load audio: {}", filepath.string());
			mAudioAssets.remove(assetHandle);
			return invalid_handle;
		}

		mFilepathCache[filepath] = assetHandle;

		AssetLoadedEvent assetLoadedEvent(assetHandle, AssetType::AUDIO);
		mEventCallback(assetLoadedEvent);

		return assetHandle;
	}
}