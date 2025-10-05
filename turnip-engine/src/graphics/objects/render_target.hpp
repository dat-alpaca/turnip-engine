#pragma once
#include "common.hpp"
#include "graphics/objects/texture.hpp"
#include <vector>

namespace tur
{
	enum class LoadOp
	{
		LOAD,
		CLEAR,
		DONT_CARE
	};

	enum class StoreOp
	{
		STORE,
		DONT_CARE
	};

	struct AttachmentDescription
	{
		TextureDescriptor textureDescriptor;
		LoadOp loadOp = LoadOp::LOAD;
		StoreOp storeOp = StoreOp::STORE;
	};

	struct RenderTargetDescriptor
	{
		std::vector<AttachmentDescription> attachmentDescriptions = {};
	};

	using render_target_handle = handle_type;
}