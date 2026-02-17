#pragma once
#include "graphics/objects/render_target.hpp"
#include "graphics/objects/texture.hpp"

namespace tur::gl
{
	struct RenderTarget
	{
		render_target_handle handle;
		std::vector<texture_handle> colorAttachments;
		texture_handle depthAttachment;
	};
}