#pragma once
#include "defines.hpp"
#include "graphics/objects/render_target.hpp"
#include "rhi/rhi.hpp"
#include "graphics/types/viewport.hpp"
#include "utils/extent.hpp"

namespace tur
{    
    class Renderer
    {
    public:
        CommandBuffer& get_command_buffer() { return commandBuffer; }

    public:
        void set_viewport(const Viewport& viewport)
        {
            this->viewport = viewport;
        }
		void set_scissor(const Extent2D& scissor)
        {
            this->scissor = scissor;
        }

        void set_render_target(render_target_handle renderTarget)
        {
            this->renderTarget = renderTarget;
        }

    protected:
        render_target_handle renderTarget = invalid_handle;
        CommandBuffer commandBuffer;
        Viewport viewport;
		Extent2D scissor;
    };
}