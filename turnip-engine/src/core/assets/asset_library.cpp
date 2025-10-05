#include "asset_library.hpp"
#include <stb_image.h>

// TODO
namespace tur
{
	AssetInformation load_texture_asset(AssetLibrary* assetLibrary, const std::filesystem::path& filepath,
										const UUID& uuid)
	{
		if (!std::filesystem::exists(filepath))
		{
			TUR_LOG_ERROR("Failed to load texture: '{}'. File does not exist.", filepath.string());
			return {};
		}

		if (assetLibrary->assetFilepathMap.find(filepath.string()) != assetLibrary->assetFilepathMap.end())
			return {assetLibrary->assetFilepathMap[filepath.string()], true};

		int width, height, channels;

		byte* data = stbi_load(filepath.string().c_str(), &width, &height, &channels, 0);
		u64 size = width * height * channels;
		std::vector<byte> buffer(data, data + size);

		stbi_image_free(data);

		TextureAsset asset;
		{
			asset.metadata.filepath = filepath;
			asset.metadata.uuid = uuid != invalid_uuid ? uuid : UUID();

			asset.width = static_cast<u32>(width);
			asset.height = static_cast<u32>(height);
			asset.channels = static_cast<u32>(channels);
			asset.data = buffer;
		}

		asset_handle textureHandle = static_cast<asset_handle>(assetLibrary->textures.add(asset));
		assetLibrary->assetFilepathMap[filepath.string()] = textureHandle;

		return {textureHandle};
	}

	AssetInformation load_texture_asset_float(AssetLibrary* assetLibrary, const std::filesystem::path& filepath,
											  const UUID& uuid)
	{
		if (!std::filesystem::exists(filepath))
		{
			TUR_LOG_ERROR("Failed to load texture: '{}'. File does not exist.", filepath.string());
			return {invalid_handle};
		}

		if (assetLibrary->assetFilepathMap.find(filepath.string()) != assetLibrary->assetFilepathMap.end())
			return {assetLibrary->assetFilepathMap[filepath.string()], true};

		int width, height, channels;
		return {};

		/*
		DataBuffer buffer;
		{
			buffer.data = stbi_loadf(filepath.string().c_str(), &width, &height, &channels, 0);
			buffer.size = width * height * channels;
		}

		TextureAsset asset;
		{
			asset.filepath = filepath;
			asset.uuid = uuid != invalid_uuid ? uuid : UUID();

			asset.width = static_cast<u32>(width);
			asset.height = static_cast<u32>(height);
			asset.channels = static_cast<u32>(channels);
			asset.data = buffer;
			asset.floatTexture = true;
		}

		asset_handle textureHandle = static_cast<asset_handle>(assetLibrary->textures.add(asset));
		assetLibrary->assetFilepathMap[filepath.string()] = textureHandle;

		return {textureHandle};
		*/
	}

	void unload_texture_asset(AssetLibrary* assetLibrary, asset_handle assetHandle)
	{
		// TODO: remove from texturefilepathmap

		// TextureAsset asset = assetLibrary->textures.remove(assetHandle);
		// stbi_image_free(asset.data.data);
	}
}