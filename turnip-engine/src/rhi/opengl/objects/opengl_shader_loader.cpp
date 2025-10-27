#include "opengl_shader_loader.hpp"
#include "utils/file.hpp"
#include <spirv_glsl.hpp>

namespace tur
{
	static u32 get_glsl_version(const WindowingCapabilities& capabilities)
	{
		return capabilities.major * 100 + capabilities.minor * 10 + capabilities.patch;
	}
}

namespace tur::gl
{
	std::string
	ShaderLoaderGL::compile(const WindowingCapabilities& capabilities, const std::filesystem::path& spirvFilepath)
	{
		try
		{
			std::vector<u8> data = read_file_binary(spirvFilepath);
			std::vector<u32> spirvData(data.size() / sizeof(u32));
			memcpy(spirvData.data(), data.data(), data.size());

			spirv_cross::CompilerGLSL compiler(spirvData);
			auto options = compiler.get_common_options();
			{
				options.version = get_glsl_version(capabilities);
				options.es = false;
			}
			compiler.set_common_options(options);

			return compiler.compile();
		}
		catch (const std::exception& e)
		{
			TUR_LOG_CRITICAL("Failed to convert SPIRV data to GLSL.");
		}
	}
}
