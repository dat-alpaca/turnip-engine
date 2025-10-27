#pragma once
#include "graphics/objects/shader_loader.hpp"
#include <any>

namespace tur::noop
{
	class ShaderLoaderNOOP
	{
	public:
		std::any compile(const WindowingCapabilities& capabilities, const std::filesystem::path& spirvFilepath)
		{
			return 0;
		}
	};
}

namespace tur
{
	using ShaderLoader = BaseShaderLoader<noop::ShaderLoaderNOOP>;
}