#include "texture_asset_binder.hpp"
#include "assets/asset_library.hpp"

namespace tur
{
	void TextureAssetBinder::initialize(RenderInterface* rhi, AssetLibrary* assetLibrary)
	{
		rRHI = rhi;
		rAssetLibrary = assetLibrary;
	}

	texture_handle TextureAssetBinder::get_texture(asset_handle handle)
	{
		TUR_ASSERT(handle >= 0, "Invalid asset handle.");
		TUR_ASSERT(handle != invalid_handle, "Invalid asset handle.");

		if (mTextureMap.find(handle) == mTextureMap.end())
		{
			TUR_LOG_WARN("Failed to retrieve texture handle: {}", handle);
			return invalid_handle;
		}

		return mTextureMap[handle];
	}
	texture_handle TextureAssetBinder::upload_texture(asset_handle assetHandle)
	{
		auto& resources = rRHI->get_resource_handler();

		TextureAsset textureAsset = rAssetLibrary->get_texture(assetHandle);

		TextureDescriptor descriptor;
		{
			descriptor.width = textureAsset.width;
			descriptor.height = textureAsset.height;
			descriptor.depth = 1;
			descriptor.mipLevels = 1;
			descriptor.samples = 1;
			descriptor.type = TextureType::TEXTURE_2D;
		}

		texture_handle textureHandle = resources.create_texture(descriptor, textureAsset);
		mTextureMap[assetHandle] = textureHandle;
		return textureHandle;
	}
}
