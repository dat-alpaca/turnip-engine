#pragma once
#include "glm/fwd.hpp"
#include "graphics/objects/texture.hpp"

namespace tur
{
    struct MetallicRoughnessMaterial
    {
        glm::vec4 baseColor;

        texture_handle albedo;
        texture_handle normal;
        texture_handle metallic;
        texture_handle roughness;
    };
}