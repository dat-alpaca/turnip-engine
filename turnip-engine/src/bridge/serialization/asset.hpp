#pragma once
#include <json.hpp>

#include "assets/font/font_asset.hpp"
#include "graphics/objects/texture.hpp"
#include "utils/uuid/uuid.hpp"

#include "core.hpp"
#include "assets/asset.hpp"
#include "assets/audio/audio_asset.hpp"
#include "assets/cubemap/cubemap_asset.hpp"
#include "assets/mesh/mesh_asset.hpp"
#include "assets/texture/texture_options.hpp"

/* Asset */
namespace tur
{   
    NLOHMANN_JSON_SERIALIZE_ENUM(AssetType,
    {
        { AssetType::NONE, "NONE" },
        { AssetType::TEXTURE, "TEXTURE" },
        { AssetType::TEXTURE_ARRAY, "TEXTURE_ARRAY" },
        { AssetType::CUBEMAP, "CUBEMAP" },
        { AssetType::AUDIO, "AUDIO" },
        { AssetType::MESH, "MESH" },
        { AssetType::FONT, "FONT" },
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(AssetLifetime,
    {
        { AssetLifetime::APPLICATION, "APPLICATION" },
        { AssetLifetime::SCENE_BOUND, "SCENEBOUND" },
    });

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AssetMetadata, filepath, type, lifetime, assetUUID);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Asset, metadata);
}

/* Texture */
namespace tur
{
    NLOHMANN_JSON_SERIALIZE_ENUM(TextureDataFormat,
    {
        { TextureDataFormat::RED,               "RED"},
        { TextureDataFormat::RG,                "RG"},
        { TextureDataFormat::RGB,               "RGB"},
        { TextureDataFormat::RGBA,              "RGBA"},
        { TextureDataFormat::BGR,               "BGR"},
        { TextureDataFormat::BGRA,              "BGRA"},
        { TextureDataFormat::RED_INTEGER,       "REDINT"},
        { TextureDataFormat::RG_INTEGER,        "RGINT"},
        { TextureDataFormat::RGB_INTEGER,       "RGBINT"},
        { TextureDataFormat::RGBA_INTEGER,      "RGBAINT"},
        { TextureDataFormat::BGR_INTEGER,       "BGRINT"},
        { TextureDataFormat::BGRA_INTEGER,      "BGRAINT"},
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(TextureFormat,
    {
        { TextureFormat::R8_UNORM,                 "R8U"},
        { TextureFormat::RG8_UNORM,                "RGU"},
        { TextureFormat::RGB8_UNORM,               "RGBU"},
        { TextureFormat::RGBA8_UNORM,              "RGBAU"},
        { TextureFormat::RGBA16_SFLOAT,            "RGBA16S"},
        { TextureFormat::DEPTH_16_UNORM,           "DEPTH16U"},
        { TextureFormat::DEPTH_STENCIL24_S8U_INT,  "DEPTHSTENCIL24S8U"},
        { TextureFormat::B8G8R8A8_UNORM,           "B8G8R8A8U"}
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(TextureType,
    {
        { TextureType::TEXTURE_2D,          "TEXTURE2D" },
        { TextureType::TEXTURE_3D,          "TEXTURE3D" },
        { TextureType::ARRAY_TEXTURE_2D,    "ARRAYTEXTURE2D" },
        { TextureType::CUBE_MAP,            "CUBEMAP" }
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(WrapMode,
    {
        { WrapMode::REPEAT,             "REPEAT" },
        { WrapMode::MIRRORED_REPEAT,    "MIRRORED_REPEAT" },
        { WrapMode::CLAMP_TO_EDGE,      "CLAMPTOEDGE" },
        { WrapMode::CLAMP_TO_BORDER,    "CLAMPTOBORDER" },
    });
	
	NLOHMANN_JSON_SERIALIZE_ENUM(FilterMode,
    {
        { FilterMode::NEAREST,   "NEAREST" },
        { FilterMode::LINEAR,    "LINEAR" },
        { FilterMode::BILINEAR,  "BILINEAR" },
        { FilterMode::TRILINEAR, "TRILINEARR" },
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(TextureTiling,
    {
        { TextureTiling::OPTIMAL,   "OPTIMAL" },
        { TextureTiling::RAW,       "RAW" },
    });
}

namespace tur
{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        TextureOptions, 
        dataFormat, floatTexture, 
        layerWidth, layerHeight,
        depth, mipLevels, samples, arrayLayers, format, type,
        wrapS, wrapT, wrapR, minFilter, magFilter,
        tiling, generateMipmaps, isTextureArray
    );

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        TextureAsset, 
        metadata, width, height, channels, options
    );

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        CubemapAsset, metadata, filepaths, width, height, channels, options
    );
}


/* Audio */
namespace tur
{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AudioAsset, metadata);
}

/* Mesh */
namespace tur
{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        MeshData, 
        vertices, indices
    );

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        MetallicRoughnessMaterialData, 
        baseColorFactor, albedoTexture, normalTexture, metallicTexture, roughnessTexture
    );

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        MeshAsset, 
        metadata
    );
}

/* Font */    
namespace tur
{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        tur::FontAsset, 
        metadata, height, options, isSDF
    );
}