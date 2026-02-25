#pragma once
#include <concepts>
#include <string>

#include "defines.hpp"
#include "graphics/graphics_api.hpp"
#include "window.hpp"
#include "config/window_config.hpp"

namespace tur
{
	template <typename T>
	concept IsWindowingSystem = requires(const WindowingCapabilities& capabilities, const WindowProperties& props)
	{
		{ T::initialize(capabilities, props) };
		{ T::shutdown() };
	};

	template <IsWindowingSystem T>
	class BaseWindowingSystem : public T
	{
	};
}
