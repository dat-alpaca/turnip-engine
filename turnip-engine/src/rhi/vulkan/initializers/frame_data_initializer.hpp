#pragma once
#include <vulkan/vulkan.hpp>

#include "common.hpp"
#include "rhi/vulkan/types/frame_data.hpp"
#include "rhi/vulkan/vulkan_state.hpp"

namespace tur::vulkan
{
	void initialize_frame_data(VulkanState& state, FrameDataHolder& holder);
}