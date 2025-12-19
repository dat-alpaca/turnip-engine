#pragma once
#include "windowing/windowing.hpp"
#include <filesystem>

namespace tur
{
	template <typename T>
	concept IsShaderLoader = requires(T t, const WindowingCapabilities& cap, const std::filesystem::path& spirvFilepath)
	{
		{ t.compile(cap, spirvFilepath) };
	};

	template <typename T>
	requires IsShaderLoader<T>
	class BaseShaderLoader : public T
	{
	};
}