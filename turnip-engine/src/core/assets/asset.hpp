#pragma once
#include "utils/uuid/uuid.hpp"
#include <filesystem>

namespace tur
{
	using asset_handle = handle_type;

	enum class AssetType
	{
		NONE = 0,
		TEXTURE,
		TEXTURE_ARRAY,
		AUDIO,
		MESH
	};
	static inline constexpr const char* get_asset_type_name(AssetType type)
	{
		if (type == AssetType::NONE)
			return "None";

		if (type == AssetType::TEXTURE)
			return "Texture";

		if (type == AssetType::TEXTURE_ARRAY)
			return "TextureArray";

		if (type == AssetType::AUDIO)
			return "Audio";

		if (type == AssetType::MESH)
			return "Mesh";

		return "Invalid";
	}

	enum class AssetLifetime
    {
        APPLICATION,
        SCENE_BOUND
    };

	struct AssetMetadata
	{
		std::filesystem::path filepath;
		UUID uuid;
		AssetType type;
		AssetLifetime lifetime = AssetLifetime::APPLICATION;
	};

	enum class AssetState
	{
		INVALID,
		UNLOADED,
		LOADING,
		READY
	};

	struct Asset
	{
		AssetMetadata metadata;
		AssetState state = AssetState::UNLOADED;
	};
}