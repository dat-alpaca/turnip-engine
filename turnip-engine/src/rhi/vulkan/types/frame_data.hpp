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
		explicit FrameDataHolder(u32 totalAllocatedFrames = 2)
		{
			mData.clear();
			mData.resize(totalAllocatedFrames);
		}

	public:
		void shutdown(vk::Device& device)
		{
			for (const auto& frame : mData)
			{
				device.destroySemaphore(frame.imageAvailableSemaphore);
				device.destroySemaphore(frame.renderFinishedSemaphore);
				device.destroyFence(frame.recordingFence);
			}

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