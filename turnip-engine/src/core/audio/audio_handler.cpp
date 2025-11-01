#include "audio_handler.hpp"
#include "assets/asset_library.hpp"

namespace tur
{
	void AudioHandler::initialize(NON_OWNING AssetLibrary* library)
	{
		// TODO: check_error()
		TUR_ASSERT(library, "Invalid AssetLibrary handle passed");
		rAssetLibrary = library;

		// Engine:
		auto engineConfig = ma_engine_config_init();

		ma_result result = ma_engine_init(nullptr, &mEngine);
		if (result != MA_SUCCESS)
			TUR_LOG_CRITICAL("Failed to create audio engine.");
	}
	void AudioHandler::shutdown()
	{
		ma_engine_uninit(&mEngine);
	}

	void AudioHandler::play(asset_handle assetHandle)
	{
		TUR_ASSERT(rAssetLibrary, "AudioHandler not initialized");
		TUR_ASSERT(assetHandle != invalid_handle, "Invalid asset handle passed to play()");

		auto& soundAsset = rAssetLibrary->get_audio_asset(assetHandle);
		if (soundAsset.state != AssetState::READY)
			return;

		ma_sound_start(&soundAsset.sound);
	}
	void AudioHandler::set_volume(asset_handle assetHandle, float volume)
	{
		TUR_ASSERT(rAssetLibrary, "AudioHandler not initialized");
		TUR_ASSERT(assetHandle != invalid_handle, "Invalid asset handle passed to play()");

		auto& soundAsset = rAssetLibrary->get_audio_asset(assetHandle);
		if (soundAsset.state != AssetState::READY)
			return;

		ma_sound_set_volume(&soundAsset.sound, volume);
	}
}
