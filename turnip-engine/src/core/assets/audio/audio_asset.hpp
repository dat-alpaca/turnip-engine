#pragma once
#include "assets/asset.hpp"

#include <miniaudio.h>

namespace tur
{
	struct AudioAsset : public Asset
	{
	public:
		AudioAsset() { metadata.type = AssetType::AUDIO; }

	public:
		ma_sound sound;
	};
}