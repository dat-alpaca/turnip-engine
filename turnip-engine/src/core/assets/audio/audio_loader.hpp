#pragma once
#include "audio/audio_handler.hpp"
#include <miniaudio.h>

namespace tur
{
	inline ma_result load_audio_task(const std::filesystem::path& filepath, ma_sound* sound, ma_engine& engine)
	{
		return ma_sound_init_from_file(
			&engine, 
			filepath.string().c_str(), 
			MA_SOUND_FLAG_ASYNC | MA_SOUND_FLAG_NO_SPATIALIZATION, 
			nullptr, 
			nullptr, 
			sound
		);
	}
}