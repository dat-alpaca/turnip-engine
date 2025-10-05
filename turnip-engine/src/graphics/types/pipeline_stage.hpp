#pragma once

namespace tur
{
	enum class PipelineStage
	{
		NONE = 0,
		VERTEX_STAGE = 1 << 0,
		FRAGMENT_STAGE = 1 << 2,
		GRAPHICS_ALL = 1 << 3
	};
}