#pragma once
#include "windowing/windowing.hpp"

namespace tur
{
	// clang-format off
	template <typename T, typename WindowType>
	concept IsGraphicsBinder = requires(T t, const WindowingCapabilities& capabilities)
	{
		{ t.pre_initialize(capabilities) };
	} && requires(T t, WindowType* window)
    {
		{ t.post_initialize(window) };
		{ t.present() };
    };
	// clang-format on

	template <typename Binder, typename WindowType>
	requires IsGraphicsBinder<Binder, WindowType>
	class BaseGraphicsBinder : public Binder
	{
	};
}