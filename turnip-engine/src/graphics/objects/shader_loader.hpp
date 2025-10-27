#pragma once
#include "windowing/windowing.hpp"
#include <filesystem>

namespace tur
{
	// clang-format off
	template <typename T>
	concept IsShaderLoader = requires(T t, const WindowingCapabilities& cap, const std::filesystem::path& spirvFilepath)
	{
		{ t.compile(cap, spirvFilepath) };
	};
	// clang-format on

	template <typename T>
	requires IsShaderLoader<T>
	class BaseShaderLoader : public T
	{
	};
}