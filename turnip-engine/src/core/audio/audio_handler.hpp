#pragma once
#include "assets/asset.hpp"
#include "common.hpp"

#include <miniaudio.h>

namespace tur
{
	class AssetLibrary;

	class AudioHandler
	{
	public:
		void initialize(NON_OWNING AssetLibrary* library);
		void shutdown();

		void play(asset_handle assetHandle);
		void set_volume(asset_handle assetHandle, float volume);

	public:
		ma_engine& get_engine() { return mEngine; }

	public:
		NON_OWNING AssetLibrary* rAssetLibrary = nullptr;
		ma_engine mEngine;
	};
}