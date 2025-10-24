#pragma once
#include <concepts>
#include <string>

#include "defines.hpp"
#include "graphics/graphics_api.hpp"
#include "window.hpp"

namespace tur
{
	struct WindowingCapabilities
	{
		GraphicsAPI api = GraphicsAPI::UNKNOWN;
		u8 major = 0;
		u8 minor = 0;
		u8 patch = 0;
		u8 variant = 0;

		bool scaleToMonitor = true;
		bool srgbCapable = true;
		bool resizable = true;
		bool startsVisible = false;
	};

	// clang-format off
	template <typename T>
	concept IsWindowingSystem = requires(const WindowingCapabilities& capabilities, const WindowProperties& props)
	{
		{ T::initialize(capabilities, props) };
		{ T::shutdown() };
	};
	// clang-format on

	template <IsWindowingSystem T>
	class BaseWindowingSystem : public T
	{
	};
}
