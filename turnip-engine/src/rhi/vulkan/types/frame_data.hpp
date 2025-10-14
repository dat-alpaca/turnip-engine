#pragma once
#include "common.hpp"
#include "data-structures/ring_buffer.hpp"

#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	struct FrameData
	{
		vk::CommandBuffer commandBuffer;
		vk::Fence recordingFence;
		vk::Semaphore imageAvailableSemaphore, renderFinishedSemaphore;
	};

	struct FrameDataHolder
	{
	public:
		void initialize(vk::Device device, vk::CommandPool commandPool, u32 totalAllocatedFrames)
		{
			shutdown(device);
			mData.resize(totalAllocatedFrames);

			// Command buffers:
			for (auto& frame : mData)
				frame.commandBuffer = allocate_single_primary_command_buffer(device, commandPool);

			// Synchonization primitives:
			for (auto& frame : mData)
				frame.recordingFence = allocate_signaled_fence(device);

			for (auto& frame : mData)
			{
				frame.imageAvailableSemaphore = allocate_semaphore(device);
				frame.renderFinishedSemaphore = allocate_semaphore(device);
			}
		}

		void shutdown(vk::Device& device)
		{
			for (const auto& frame : mData)
			{
				if (frame.imageAvailableSemaphore)
					device.destroySemaphore(frame.imageAvailableSemaphore);

				if (frame.renderFinishedSemaphore)
					device.destroySemaphore(frame.renderFinishedSemaphore);

				if (frame.recordingFence)
					device.destroyFence(frame.recordingFence);
			}

			mImageBufferIndex = 0;
			mData.clear();
		}

	public:
		void advance() noexcept { mData.advance(); }

	public:
		constexpr inline std::vector<FrameData>& get_frames() { return mData.get_data(); }
		constexpr inline FrameData& get_current_frame_data() { return mData.get_current(); }

		inline void set_image_buffer_index(u32 index) { mImageBufferIndex = index; }
		inline u32 get_image_buffer_index() { return mImageBufferIndex; }

	private:
		static_ring_buffer<FrameData> mData;
		u32 mImageBufferIndex = 0;
	};
}