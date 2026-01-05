#pragma once
#include "assets/texture/texture_options.hpp"
#include "data-structures/free_list.hpp"
#include "texture_asset.hpp"

#include <optional>
#include <stb_image.h>

namespace tur
{
	using texture_asset_opt = std::optional<TextureAsset>;

	inline texture_asset_opt load_texture_task(const std::filesystem::path& filepath)
	{
		if (!std::filesystem::exists(filepath))
		{
			TUR_LOG_ERROR("Failed to load texture: '{}'. File does not exist.", filepath.string());
			return std::nullopt;
		}

		int width, height, channels;

		byte* data = stbi_load(filepath.string().c_str(), &width, &height, &channels, 0);

		u64 size = width * height * channels;
		std::vector<byte> buffer(data, data + size);
		stbi_image_free(data);

		TextureAsset asset = {};
		{
			asset.data = buffer;
			asset.width = static_cast<u32>(width);
			asset.height = static_cast<u32>(height);
			asset.channels = static_cast<u32>(channels);
		}

		return asset;
	}

	inline texture_asset_opt load_float_texture_task(const std::filesystem::path& filepath)
	{
		if (!std::filesystem::exists(filepath))
		{
			TUR_LOG_ERROR("Failed to load texture: '{}'. File does not exist.", filepath.string());
			return std::nullopt;
		}

		int width, height, channels;

		float* data = stbi_loadf(filepath.string().c_str(), &width, &height, &channels, 0);

		u64 size = width * height * channels * sizeof(float);
		std::vector<byte> buffer(data, data + size);
		stbi_image_free(data);

		TextureAsset asset = {};
		{
			asset.data = buffer;
			asset.width = static_cast<u32>(width);
			asset.height = static_cast<u32>(height);
			asset.channels = static_cast<u32>(channels);
		}

		return asset;
	}
}