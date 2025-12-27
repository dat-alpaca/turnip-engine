#pragma once
#include "data-structures/ring_buffer.hpp"
#include "rhi/vulkan/allocators/allocators.hpp"
#include "rhi/vulkan/allocators/semaphore.hpp"
#include "vulkan/vulkan.hpp"

#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	struct FrameData
	{
		vk::CommandBuffer commandBuffer;
		vk::Fence recordingFence;
		vk::Semaphore imageReadySemaphore;
	};

	struct FrameDataHolder
	{
	public:
		void initialize(vk::Device device, vk::CommandPool commandPool, u32 totalAllocatedFrames)
		{
			shutdown(device);
			mData.resize(totalAllocatedFrames);
			mRenderFinishedSemaphores.resize(totalAllocatedFrames);

			// Command buffers:
			for (auto& frame : mData)
				frame.commandBuffer = allocate_single_primary_command_buffer(device, commandPool);

			// Synchonization primitives:
			for (auto& frame : mData)
			{
				frame.recordingFence = allocate_signaled_fence(device);
				frame.imageReadySemaphore = allocate_semaphore(device);
			}

			for (auto& renderFinishedSemaphore : mRenderFinishedSemaphores)
				renderFinishedSemaphore = allocate_semaphore(device);
		}

		void shutdown(vk::Device& device)
		{
			for (const auto& frame : mData)
			{
				if (frame.recordingFence)
					device.destroyFence(frame.recordingFence);

				if (frame.imageReadySemaphore)
					device.destroySemaphore(frame.imageReadySemaphore);
			}

			for (auto& renderFinishedSemaphore : mRenderFinishedSemaphores)
			{
				if(renderFinishedSemaphore)
					device.destroySemaphore(renderFinishedSemaphore);
			}	

			mRenderFinishedSemaphores.clear();
			mImageBufferIndex = 0;
			mData.clear();
		}

	public:
		void advance() noexcept { mData.advance(); }

	public:
		constexpr inline vk::Semaphore get_render_finished_semaphore() const {
			 return mRenderFinishedSemaphores[mImageBufferIndex];
		}

		constexpr inline std::vector<FrameData>& get_frames() { return mData.get_data(); }
		constexpr inline FrameData& get_current_frame_data() { return mData.get_current(); }

		inline void set_image_buffer_index(u32 index) { mImageBufferIndex = index; }
		inline u32 get_image_buffer_index() { return mImageBufferIndex; }

	private:
		std::vector<vk::Semaphore> mRenderFinishedSemaphores;
		static_ring_buffer<FrameData> mData;
		u32 mImageBufferIndex = 0;
	};
}