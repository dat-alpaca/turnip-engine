#pragma once
#include <vector>

namespace tur::vulkan
{
	struct ValidationResults
	{
		bool success = false;
		std::vector<const char*> unsupported;
	};
}