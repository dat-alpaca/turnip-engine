#pragma once
#include "frame_data.hpp"
#include <turnip_engine.hpp>

using namespace tur;

class ComputePass
{
public:
    void initialize(NON_OWNING RenderInterface* rhi, u32 swapchainImageCount = 3)
    {
        rRHI = rhi;

        mCommandBuffer = rRHI->create_command_buffer();
		mFrameDataHolder.initialize(rRHI, swapchainImageCount);
    }

    void shutdown()
    {
        mFrameDataHolder.shutdown();
    }

	void on_render_begin()
	{
		mCurrentFrame = &mFrameDataHolder.get_current_frame_data();

		rRHI->begin_frame(mCurrentFrame->inFlightFence);
		rRHI->reset_fence(mCurrentFrame->inFlightFence);

		mCommandBuffer.reset(mCurrentFrame->commandBuffer);

		mCommandBuffer.begin();
	}

	void on_render_end()
	{
		mCommandBuffer.execute_secondary_buffers(mCommandBuffers);
		mCommandBuffer.end();

		rRHI->end_frame();
		rRHI->submit(
			rRHI->get_queue(QueueOperation::COMPUTE), 
            mCurrentFrame->commandBuffer, 
            mCurrentFrame->inFlightFence,
            {},
			mCurrentFrame->imageReadySemaphore
		);

		mCommandBuffers.clear();
	}

public:
	void add_command_buffer(const CommandBuffer::BufferType& buffer)
    {
        mCommandBuffers.push_back(buffer);
    }

public:
    void set_frame_index(u32 index) { mFrameDataHolder.set_ring_data_index(index); }
	FrameData& get_current_frame_data() { return mFrameDataHolder.get_current_frame_data(); }

private:
	NON_OWNING RenderInterface* rRHI = nullptr;
	
private:
    CommandBuffer mCommandBuffer;
    FrameDataHolder mFrameDataHolder;
	
	FrameData* mCurrentFrame = nullptr;
    std::vector<CommandBuffer::BufferType> mCommandBuffers;
};