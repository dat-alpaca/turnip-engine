#pragma once
#include "graphics_glfw.hpp"
#include "window_glfw.hpp"
#include "windowing/windowing.hpp"

namespace tur::platform
{
	class WindowingGLFW
	{
	public:
		static tur_unique<Window> initialize(const WindowingCapabilities& capabilities,
											 const WindowProperties& properties);
		static void shutdown();

	public:
		static GraphicsBinder& get_binder() { return sBinder; }

	private:
		static inline GraphicsBinder sBinder;
	};

	using WindowingSystem = BaseWindowingSystem<WindowingGLFW>;
}
