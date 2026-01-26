#pragma once
#include "assets/cubemap/cubemap_asset.hpp"
#include "assets/texture/texture_options.hpp"
#include "data-structures/free_list.hpp"
#include "texture_asset.hpp"

#include <cstring>
#include <iterator>
#include <optional>
#include <stb_image.h>
#include <stb_image_write.h>
#include <string>

namespace tur
{
	using texture_asset_opt = std::optional<TextureAsset>;

	inline texture_asset_opt load_texture_task(const std::filesystem::path& filepath, u32 desiredChannels = 0)
	{
		if (!std::filesystem::exists(filepath))
		{
			TUR_LOG_ERROR("Failed to load texture: '{}'. File does not exist.", filepath.string());
			return std::nullopt;
		}

		int width, height, channels;

		byte* data = stbi_load(filepath.string().c_str(), &width, &height, &channels, desiredChannels);

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

	inline texture_asset_opt load_float_texture_task(const std::filesystem::path& filepath, u32 desiredChannels = 0)
	{
		if (!std::filesystem::exists(filepath))
		{
			TUR_LOG_ERROR("Failed to load texture: '{}'. File does not exist.", filepath.string());
			return std::nullopt;
		}

		int width, height, channels;

		float* data = stbi_loadf(filepath.string().c_str(), &width, &height, &channels, desiredChannels);

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

	inline std::optional<CubemapAsset> load_cubemap_task(const std::array<std::filesystem::path, 6>& filepaths)
	{
		CubemapAsset asset = {};
		if(filepaths.size() != 6)
		{
			TUR_LOG_ERROR("Failed to load cubemap texture. Inadequate amount of images.");
			return std::nullopt;
		}
		
		u64 layerSize = 0;
		u32 i = 0;
		bool parametersSet = false;
		for(const auto& filepath : filepaths)
		{
			if (!std::filesystem::exists(filepath))
			{
				TUR_LOG_ERROR("Failed to load cubemap texture: '{}'. File does not exist.", filepath.string());
				return std::nullopt;
			}

			int width, height, channels;
			stbi_set_flip_vertically_on_load(true);
			byte* data = stbi_load(filepath.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);

			layerSize = width * height * 4 * sizeof(byte);

			if(!parametersSet)
			{
				parametersSet = true;
				asset.width = static_cast<u32>(width);
				asset.height = static_cast<u32>(height);
				asset.channels = static_cast<u32>(channels);
				
				u64 totalSize = 6 * layerSize;
				asset.data.reserve(totalSize);

				std::copy(&data[0], &data[layerSize], std::back_inserter(asset.data));
			}
			else
			{
				if( asset.width    != static_cast<u32>(width)   || 
					asset.height   != static_cast<u32>(height)  ||
				    asset.channels != static_cast<u32>(channels)  )
				{
					TUR_LOG_ERROR("Failed to load cubemap texture: '{}'. Texture dimensions differ.");
					return std::nullopt;
				}

				std::copy(&data[0], &data[layerSize], std::back_inserter(asset.data));
			}

			stbi_image_free(data);
			++i;
		}

		return asset;
	}
}