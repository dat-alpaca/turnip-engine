#pragma once
#include "graphics/objects/shader_loader.hpp"

namespace tur::gl
{
	class ShaderLoaderGL
	{
	public:
		std::string compile(const WindowingCapabilities& capabilities, const std::filesystem::path& spirvFilepath);
	};
}

namespace tur
{
	using ShaderLoader = BaseShaderLoader<gl::ShaderLoaderGL>;
}