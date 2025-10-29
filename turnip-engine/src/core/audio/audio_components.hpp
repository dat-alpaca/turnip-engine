#pragma once
#include "audio/audio.hpp"

namespace tur
{
	struct AudioSourceComponent
	{
	public:
		AudioSourceComponent() = default;
		AudioSourceComponent(audio_handle audioHandle)
			: audioHandle(audioHandle)
		{
		}

	public:
		audio_handle audioHandle = invalid_handle;
	};
}