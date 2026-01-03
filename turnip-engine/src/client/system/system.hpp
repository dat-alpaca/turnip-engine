#pragma once
#include "memory/memory.hpp"
#include "scene/scene.hpp"

namespace tur
{
    struct System
    {
    public:
        void set_scene(NON_OWNING Scene* scene)
        {
            rScene = scene;
        }

    public:
		Scene* get_scene() const { return rScene; }
    
    protected:
        NON_OWNING Scene* rScene = nullptr;
    };
}