#pragma once
#include <filesystem>

#include "defines.hpp"
#include "logging/logging.hpp"
#include "utils/string_utils.hpp"
#include "utils/uuid/uuid.hpp"

/* Asset Type */
namespace tur
{
	using asset_handle = handle_type;

	enum class AssetType
	{
		NONE = 0,
		TEXTURE,
		TEXTURE_ARRAY,
		CUBEMAP,
		AUDIO,
		MESH,
		FONT
	};

	static inline AssetType get_asset_type_value(const std::string& name)
	{
		std::string converted = name;
		to_upper(converted);

        if (converted == "TEXTURE")
			return AssetType::TEXTURE;

        if (converted == "TEXTUREARRAY")
			return AssetType::TEXTURE_ARRAY;

        if (converted == "CUBEMAP")
			return AssetType::CUBEMAP;

        if (converted == "AUDIO")
			return AssetType::AUDIO;

        if (converted == "MESH")
			return AssetType::MESH;

        if (converted == "FONT")
			return AssetType::FONT;

		return AssetType::NONE;
	}

	static inline constexpr const char* get_asset_type_name(AssetType type)
	{
		if (type == AssetType::TEXTURE)
			return "texture";

		if (type == AssetType::TEXTURE_ARRAY)
			return "texturearray";

		if (type == AssetType::AUDIO)
			return "audio";

		if (type == AssetType::MESH)
			return "mesh";

		return "none";
	}
}

/* Asset Lifetime */
namespace tur
{
	enum class AssetLifetime
    {
        APPLICATION,
        SCENE_BOUND
    };

	static inline AssetLifetime get_asset_lifetime_value(const std::string& name)
	{
		std::string converted = name;
		to_upper(converted);

        if (name == "APPLICATION")
			return AssetLifetime::APPLICATION;

        if (name == "SCENEBOUND")
			return AssetLifetime::SCENE_BOUND;

		TUR_LOG_ERROR("Invalid lifetime passed. Default: APPLICATION");
		return AssetLifetime::APPLICATION;
	}

	static inline constexpr const char* get_asset_lifetime_name(AssetLifetime lifetime)
	{
		if (lifetime == AssetLifetime::APPLICATION)
			return "application";

		if (lifetime == AssetLifetime::SCENE_BOUND)
			return "scenebound";
	
		TUR_LOG_ERROR("Invalid lifetime passed. Default: APPLICATION");
		return "application";
	}
}

/* Asset State */
namespace tur
{
	enum class AssetState
	{
		INVALID,
		UNLOADED,
		LOADING,
		READY
	};

	static inline AssetState get_asset_state_value(const std::string& name)
	{
		std::string converted = name;
		to_upper(converted);

        if (name == "UNLOADED")
			return AssetState::UNLOADED;

        if (name == "LOADING")
			return AssetState::LOADING;

		if (name == "READY")
			return AssetState::READY;

		return AssetState::INVALID;
	}

	static inline constexpr const char* get_asset_state_name(AssetState state)
	{
		if (state == AssetState::UNLOADED)
			return "unloaded";

		if (state == AssetState::LOADING)
			return "loading";

		if (state == AssetState::READY)
			return "ready";
	
		return "invalid";
	}
}

namespace tur
{
	struct AssetMetadata
	{
		std::filesystem::path filepath;
		uuid assetUUID;
		AssetType type;
		AssetLifetime lifetime = AssetLifetime::APPLICATION;
	};

	struct Asset
	{
		AssetMetadata metadata;
		FIELD_SET_INTERNALLY AssetState state = AssetState::UNLOADED;
	};
}