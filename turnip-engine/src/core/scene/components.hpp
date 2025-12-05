#pragma once
#include "audio/audio_components.hpp"
#include "common_components.hpp"
#include "graphics/components.hpp"
#include "physics/physics_components.hpp"
#include "script/script_components.hpp"

namespace tur
{
    using AllComponents = std::tuple<
        AudioSourceComponent, 

        HierarchyComponent, UUIDComponent, NameComponent, TransformComponent,

        Sprite2DComponent, 

        // TODO: Body2DComponent

        ScriptComponent
    >;
}