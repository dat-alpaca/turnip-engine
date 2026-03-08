#pragma once
#include "defines.hpp"
#include "frame_data.hpp"

#include "graphics/objects/sync.hpp"
#include "graphics/render_interface.hpp"
#include "graphics/types/pipeline_stage.hpp"

#include <turnip_engine.hpp>
#include <vector>

using namespace tur;

class GraphicsPass
{
public:
    void initialize(NON_OWNING RenderInterface* rhi, u32 swapchainImageCount = 3)
    {
        rRHI = rhi;
        mSwapchainImageCount = swapchainImageCount;

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

        auto imageResult = rRHI->acquire_swapchain_image(mCurrentFrame->imageReadySemaphore);
        if(!imageResult.has_value())
            return mFrameDataHolder.initialize(rRHI, mSwapchainImageCount);

        mImageResult = *imageResult;
        mFrameDataHolder.set_image_buffer_index(mImageResult);

		rRHI->reset_fence(mCurrentFrame->inFlightFence);

        mCommandBuffer.reset(mCurrentFrame->commandBuffer);
        mCommandBuffer.set_clear_color({}, ClearFlags::COLOR | ClearFlags::DEPTH);

        mCommandBuffer.begin();
        mCommandBuffer.begin_rendering();
	}
    void on_render_end()
    {
        if(!mCommandBuffers.empty())
            mCommandBuffer.execute_secondary_buffers(mCommandBuffers);
		
        mCommandBuffer.end_rendering();
		mCommandBuffer.blit(mImageResult);
		mCommandBuffer.end();

        rRHI->end_frame();
		rRHI->submit(
			rRHI->get_queue(QueueOperation::GRAPHICS), 
			mCurrentFrame->commandBuffer, 
			mCurrentFrame->inFlightFence, 
			get_submit_semaphores(mCurrentFrame->imageReadySemaphore, PipelineStageFlags::COLOR_ATTACH_OUTPUT),
			mFrameDataHolder.get_render_finished_semaphore()
		);

		rRHI->present
        (
			rRHI->get_queue(QueueOperation::PRESENT),
			{  mFrameDataHolder.get_render_finished_semaphore() },
			mImageResult 
		);

        mCommandBuffers.clear();
        mExtraData.clear();
    }

public:
    void add_wait_semaphore(semaphore_handle semaphore, PipelineStageFlags flag)
    {
        mExtraData.push_back({ semaphore, flag });
    }
    void add_command_buffer(const CommandBuffer::BufferType& buffer)
    {
        mCommandBuffers.push_back(buffer);
    }

public:
    u32 get_frame_index() const { return mFrameDataHolder.get_frame_index(); }
    void advance() { mFrameDataHolder.advance(); }

private:
    inline std::vector<SubmitSemaphoreData> get_submit_semaphores(semaphore_handle firstSemaphore, PipelineStageFlags firstFlag) const
    {
        std::vector<SubmitSemaphoreData> data;
        data.reserve(1 + mExtraData.size());

        data.push_back({ firstSemaphore, firstFlag });

        for(const auto& extra : mExtraData)
            data.push_back(extra);

        return data;
    }

private:
	NON_OWNING RenderInterface* rRHI = nullptr;
	
private:
    CommandBuffer mCommandBuffer;
    FrameDataHolder mFrameDataHolder;
    FrameData* mCurrentFrame = nullptr;

    std::vector<SubmitSemaphoreData> mExtraData;
    std::vector<CommandBuffer::BufferType> mCommandBuffers;

private:
    u32 mSwapchainImageCount = 3;
    u32 mImageResult = invalid_handle;
};