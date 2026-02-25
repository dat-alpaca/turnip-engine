#pragma once
#include <glm/glm.hpp>
#include "common.hpp"

namespace
{
	static std::string default_window_title()
	{
		return "TurnipEngine v" + std::to_string(TUR_ENGINE_VERSION_MAJOR) + "."
			   + std::to_string(TUR_ENGINE_VERSION_MINOR);
	}
}

namespace tur
{
	struct WindowingCapabilities
	{
		u8 major = 0;
		u8 minor = 0;
		u8 patch = 0;
		u8 variant = 0;

		bool scaleToMonitor = true;
		bool srgbCapable = true;
		bool resizable = true;
		bool startsVisible = false;
	};

	struct WindowProperties
	{
		enum Position : int
		{
			DEFAULT = -1
		};

	public:
		std::string title = default_window_title();
		glm::uvec2 dimensions = glm::vec2(640, 480);
		glm::uvec2 position = glm::vec2(Position::DEFAULT, Position::DEFAULT);
		glm::uvec2 minSize = glm::vec2(0);
		glm::uvec2 maxSize = dimensions;
		bool startsMaximized = false;
	};
}