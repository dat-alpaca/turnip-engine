#pragma once
#include "assets/asset.hpp"
#include "defines.hpp"
#include <miniaudio.h>

namespace tur
{
	struct AudioAsset : public Asset
	{
	public:
		AudioAsset() { metadata.type = AssetType::AUDIO; }

	public:
		FIELD_SET_INTERNALLY ma_sound sound;
	};
}