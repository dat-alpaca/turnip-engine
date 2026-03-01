#pragma once
#include "rhi/vulkan/objects/pipeline.hpp"
#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	class RenderInterfaceVulkan;

	Pipeline allocate_graphics_pipeline(RenderInterfaceVulkan* rhi, const PipelineDescriptor& descriptor);
	Pipeline allocate_compute_pipeline(RenderInterfaceVulkan* rhi, const ComputePipelineDescriptor& descriptor);
}