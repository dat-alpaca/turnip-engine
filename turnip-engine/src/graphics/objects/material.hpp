#pragma once
#include "defines.hpp"
#include "glm/fwd.hpp"
#include "graphics/objects/texture.hpp"

namespace tur
{
    struct MetallicRoughnessMaterial
    {
        glm::vec4 baseColor;

        texture_handle albedo = invalid_handle;
        texture_handle normal = invalid_handle;
        texture_handle metallic = invalid_handle;
        texture_handle roughness = invalid_handle;

        bool is_valid()
        {
            return albedo != invalid_handle && normal != invalid_handle && metallic != invalid_handle &&
                roughness != invalid_handle;
        }
    };
}