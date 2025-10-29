#pragma once
#include "common.hpp"
#include <filesystem>
#include <miniaudio.h>

namespace tur
{
	class AudioAssetHandler
	{
	public:
		void initialize()
		{
			ma_engine_config engineConfig;
			{
				engineConfig = ma_engine_config_init();
			}

			ma_result result = ma_engine_init(&engineConfig, &mEngine);
			if (result != MA_SUCCESS)
			{
				TUR_LOG_CRITICAL("Failed to initialize the audio engine");
				return;
			}
		}

		void load(const std::filesystem::path& filepath)
		{
			// MA_SOUND_FLAG_NO_SPATIALIZATION
			// MA_SOUND_FLAG_NO_PITCH

			ma_result result;
			ma_sound sound;

			result = ma_sound_init_from_file(&mEngine, filepath.string().c_str(), 0, NULL, NULL, &sound);
			if (result != MA_SUCCESS)
				return TUR_LOG_ERROR("Failed to load audio file at: {}", filepath.string());
		}

	private:
		ma_engine mEngine;
	};
}