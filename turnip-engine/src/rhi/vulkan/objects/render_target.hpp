#pragma once
#include "graphics/objects/render_target.hpp"

namespace tur::vulkan
{
	struct RenderAttachment
	{
		texture_handle textureHandle = invalid_handle;
		StoreOp storeOp;
		LoadOp loadOp;
	};

	// TODO: support multiple color attachments:
	struct RenderTarget
	{
		RenderAttachment colorAttachment;
		RenderAttachment depthAttachment;
	};
}