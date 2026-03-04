#pragma once
#include "data-structures/ring_buffer.hpp"
#include "graphics/objects/sync.hpp"
#include <turnip_engine.hpp>

using namespace tur;

struct FrameData
{
	command_buffer_handle commandBuffer; 
	fence_handle inFlightFence;
	semaphore_handle imageReadySemaphore;
};

struct FrameDataHolder
{
public:
	void initialize(RenderInterface* rhi, u32 totalAllocatedFrames)
	{
		auto& resources = rhi->get_resource_handler();
		rRHI = rhi;

		shutdown();
	
		mData.resize(totalAllocatedFrames);
		mRenderFinishedSemaphores.resize(totalAllocatedFrames);

		// Command buffers:
		for (auto& frame : mData)
			frame.commandBuffer = resources.create_primary_command_buffer();

		// Synchonization primitives:
		for (auto& frame : mData)
		{
			frame.inFlightFence = resources.create_fence();
			frame.imageReadySemaphore = resources.create_semaphore();
		}

		for (auto& renderFinishedSemaphore : mRenderFinishedSemaphores)
			renderFinishedSemaphore = resources.create_semaphore();
	}

	void shutdown()
	{
		auto& resources = rRHI->get_resource_handler();

		for (const auto& frame : mData)
		{
			if (frame.inFlightFence)
				resources.destroy_fence(frame.inFlightFence);

			if (frame.imageReadySemaphore)
				resources.destroy_semaphore(frame.imageReadySemaphore);
		}

		for (auto& renderFinishedSemaphore : mRenderFinishedSemaphores)
		{
			if(renderFinishedSemaphore)
				resources.destroy_semaphore(renderFinishedSemaphore);
		}	

		mRenderFinishedSemaphores.clear();
		mImageBufferIndex = 0;
		mData.clear();
	}

public:
	void advance() noexcept { mData.advance(); }

public:
	constexpr inline semaphore_handle get_render_finished_semaphore() const {
		 return mRenderFinishedSemaphores[mImageBufferIndex];
	}

	constexpr inline std::vector<FrameData>& get_frames() { return mData.get_data(); }
	constexpr inline FrameData& get_current_frame_data() { return mData.get_current(); }
	constexpr inline u32 get_frame_index() const { return mData.current_index(); }

	inline void set_ring_data_index(u32 index) { mData.set_current(index); }
	inline void set_image_buffer_index(u32 index) { mImageBufferIndex = index; }
	inline u32 get_image_buffer_index() { return mImageBufferIndex; }

private:
	std::vector<semaphore_handle> mRenderFinishedSemaphores;
	static_ring_buffer<FrameData> mData;
	u32 mImageBufferIndex = 0;

	RenderInterface* rRHI = nullptr;
};