#pragma once
#include "event/events.hpp"
#include "rhi/rhi.hpp"

namespace tur
{
	class AssetLibrary;

	class TextureAssetBinder
	{
	public:
		void initialize(RenderInterface* rhi, AssetLibrary* assetLibrary);

	public:
		texture_handle get_texture(asset_handle handle);
		texture_handle upload_texture(asset_handle assetHandle);

	private:
		NON_OWNING RenderInterface* rRHI = nullptr;
		NON_OWNING AssetLibrary* rAssetLibrary = nullptr;

	private:
		std::unordered_map<asset_handle, texture_handle> mTextureMap;
	};
}