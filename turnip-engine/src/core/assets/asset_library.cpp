#include "asset_library.hpp"
#include <stb_image.h>

namespace tur
{
	void AssetLibrary::initialize(WorkerPool* workerPool, TextureAssetBinder* binder)
	{
		rWorkerPool = workerPool;
		rBinder = binder;
	}
	void AssetLibrary::set_event_callback(EventCallback& eventCallback)
	{
		mEventCallback = eventCallback;
	}

	asset_handle AssetLibrary::load_texture(const std::filesystem::path& filepath)
	{
		asset_handle allocationHandle = mTextures.add({});

		rWorkerPool->submit<asset_handle>(
			[this, filepath, allocationHandle]() { return update_texture_data(filepath, allocationHandle); },
			[&](asset_handle assetHandle)
			{
				AssetLoadedEvent loadedEvent(assetHandle);
				mEventCallback(loadedEvent);
			}
		);

		return allocationHandle;
	}
	void AssetLibrary::load_texture_imm(const std::filesystem::path& filepath, texture_handle& target)
	{
		asset_handle allocationHandle = mTextures.add({});

		rWorkerPool->submit<asset_handle>(
			[this, filepath, allocationHandle]() { return update_texture_data(filepath, allocationHandle); },
			[this, &target](asset_handle assetHandle)
			{
				if (assetHandle == invalid_handle)
				{
					TUR_LOG_ERROR("load_texture_imm returned an invalid asset handle on update_texture_data");
					return;
				}
				target = rBinder->upload_texture(assetHandle);
			}
		);
	}
	void AssetLibrary::unload_texture(asset_handle handle)
	{
		TextureAsset asset = mTextures.remove(handle);
		mAssetFilepathMap.erase(asset.metadata.filepath);

		AssetUnloadedEvent unloadedEvent(handle);
		mEventCallback(unloadedEvent);
	}
	TextureAsset AssetLibrary::get_texture(asset_handle handle)
	{
		return mTextures.get(handle);
	}

	asset_handle AssetLibrary::update_texture_data(const std::filesystem::path& filepath, asset_handle target)
	{
		// TODO: support for RGB images

		if (mAssetFilepathMap.find(filepath.string()) != mAssetFilepathMap.end())
			return mAssetFilepathMap[filepath.string()];

		if (!std::filesystem::exists(filepath))
		{
			TUR_LOG_ERROR("Failed to load texture: '{}'. File does not exist.", filepath.string());
			return invalid_handle;
		}

		int width, height, channels;
		byte* data = stbi_load(filepath.string().c_str(), &width, &height, &channels, 0);

		u64 size = width * height * channels;
		std::vector<byte> buffer(data, data + size);
		stbi_image_free(data);

		TextureAsset& asset = mTextures.get(target);
		{
			asset.data = buffer;
			asset.metadata.filepath = filepath;
			asset.metadata.uuid = UUID();

			asset.width = static_cast<u32>(width);
			asset.height = static_cast<u32>(height);
			asset.channels = static_cast<u32>(channels);
		}

		mAssetFilepathMap[filepath.string()] = target;
		return target;
	}
}