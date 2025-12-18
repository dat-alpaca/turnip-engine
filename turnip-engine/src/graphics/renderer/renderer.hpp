#pragma once
#include "rhi/rhi.hpp"
#include "graphics/types/viewport.hpp"
#include "graphics/types/extent.hpp"

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

    protected:
        CommandBuffer commandBuffer;
        Viewport viewport;
		Extent2D scissor;
    };
}