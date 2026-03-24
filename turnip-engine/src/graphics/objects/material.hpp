#pragma once
#include "defines.hpp"
#include "glm/fwd.hpp"
#include "graphics/objects/descriptor.hpp"
#include "graphics/objects/pipeline.hpp"
#include "graphics/objects/texture.hpp"

namespace tur
{
    using material_handle = handle_type;

    struct Material
    {
        pipeline_handle pipeline;
        descriptor_set_layout_handle setLayout;
    };
}