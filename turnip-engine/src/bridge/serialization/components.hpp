#pragma once
#include <json.hpp>

#include "bridge/serialization/core.hpp"
#include "scene/components.hpp"

/* Audio */
namespace tur
{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        AudioSourceComponent, 
        uuid
    );
}

/* Common */
namespace tur
{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        HierarchyComponent, 
        parent, level
    );

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        UUIDComponent, 
        uuid
    );

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        NameComponent, 
        name
    );

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        Transform, 
        position, rotation, scale
    );

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        Camera, 
        position, 
        target
    );

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        TransformComponent, 
        transform
    );

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        CameraComponent, 
        mainCamera, camera
    );
}

/* Tile */
namespace tur
{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        TileFlags, 
        data
    );

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        Tile, 
        position, layer, flags
    );

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        TilemapChunk, 
        chunks
    );

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        Tilemap2DComponent, 
        worldData, tilesPerChunk, tilePixelSize, uuid
    );
}

/* Physics */
namespace tur
{
    NLOHMANN_JSON_SERIALIZE_ENUM( BodyType,
    {
        { BodyType::STATIC,     "STATIC" },
        { BodyType::KINEMATIC,  "KINEMATIC" },
        { BodyType::DYNAMIC,    "DYNAMIC" },
    });

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        Body2DComponent, 
        type, isBullet
    );

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        RectCollider2D, 
        width, height, density, friction, restitution, rollingResistance
    );
}


/* Script */
namespace tur
{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        tur::ScriptComponent, 
        filepath
    );
}


/* Mesh */
namespace tur
{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        tur::MeshComponent, 
        uuid
    );
}


/* Texture */
namespace tur
{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        tur::Sprite2DComponent, 
        uuid
    );

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        tur::CubemapComponent, 
        uuid
    );
}


/* Text */
namespace tur
{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        tur::TextComponent, 
        uuid, text, color
    );
}
