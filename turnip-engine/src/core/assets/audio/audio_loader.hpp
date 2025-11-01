#pragma once
#include "audio/audio_handler.hpp"
#include "audio_asset.hpp"
#include "utils/file.hpp"

#include <miniaudio.h>
#include <optional>

namespace tur
{
	using ma_sound_opt = std::optional<ma_sound>;

	inline ma_sound_opt load_audio_task(const std::filesystem::path& filepath, ma_engine& engine)
	{
		ma_sound sound;
		ma_result result =
			ma_sound_init_from_file(&engine, filepath.string().c_str(), MA_SOUND_FLAG_ASYNC, nullptr, nullptr, &sound);

		if (result != MA_SUCCESS)
			return std::nullopt;

		return sound;
	}
}