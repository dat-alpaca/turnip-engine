#include "frame_data_initializer.hpp"
#include "rhi/vulkan/allocators/command_buffer.hpp"
#include "rhi/vulkan/allocators/fence.hpp"
#include "rhi/vulkan/allocators/semaphore.hpp"

namespace tur::vulkan
{
	// TODO: set a flag in frame data holder indicating it's been initialized.
	// TODO: private constructor
	void initialize_frame_data(VulkanState& state, FrameDataHolder& holder)
	{
		// Swapchain Images:
		holder = FrameDataHolder(static_cast<u32>(state.swapChainImages.size()));

		// Command buffers:
		for (auto& frame : holder.get_frames())
			frame.commandBuffer = allocate_single_primary_command_buffer(state.logicalDevice, state.commandPool);

		// Synchonization primitives:
		for (auto& frame : holder.get_frames())
			frame.recordingFence = allocate_signaled_fence(state.logicalDevice);

		for (auto& frame : holder.get_frames())
		{
			frame.imageAvailableSemaphore = allocate_semaphore(state.logicalDevice);
			frame.renderFinishedSemaphore = allocate_semaphore(state.logicalDevice);
		}
	}
}