#pragma once
#include "assets/texture/texture_asset.hpp"
#include "common.hpp"

#include "rhi/opengl/objects/render_target.hpp"
#include "rhi/opengl/opengl_common.hpp"
#include <glad/gl.h>

namespace tur::gl
{
	inline RenderTarget allocate_render_target(const RenderTargetDescriptor& descriptor)
	{
		u32 framebufferID;
		glCreateFramebuffers(1, &framebufferID);

		RenderTarget target;
		{
			target.handle = framebufferID;
		}
		return target;
	}
}