#pragma once
#include "config/config_data.hpp"
#include "core/event/events.hpp"

#include "defines.hpp"

#include "objects/sync.hpp"
#include "objects/queue.hpp"
#include "objects/image.hpp"
#include "objects/command_buffer.hpp"
#include "types/queue_operations.hpp"

#include "platform/platform.hpp"

namespace tur
{
	struct SubmitSemaphoreData
	{
		semaphore_handle waitFor;
		PipelineStageFlags stageFlag;
	};
}

namespace tur
{
	template <typename T>
	concept IsRenderInterface = requires(T t, const ConfigData& configData, Window& window, Event& event)
	{
		{ t.initialize(configData, window) };
		{ t.shutdown() };
		{ t.on_event(event) };
	} && 
	
	requires(T t, fence_handle inFlightFence, semaphore_handle signal, u32 timeout)
	{
		{ t.begin_frame(inFlightFence, timeout) };
		{ t.acquire_swapchain_image(signal, timeout) } 		-> std::same_as<std::optional<image_handle>>;
		{ t.reset_fence(inFlightFence) };		
	} &&

	requires(T t, queue_handle q, command_buffer_handle cb, fence_handle inFlightFence, semaphore_handle signal,
		     const std::vector<SubmitSemaphoreData>& semaphoreData)
	{
		{ t.submit(q, cb, inFlightFence, semaphoreData, signal) };
	} &&

	requires(T t, queue_handle queue, u32 imageHandle, const std::vector<semaphore_handle>& waitFor)
	{
		{ t.present(queue, waitFor, imageHandle) };
		{ t.end_frame() };
	} &&

	requires(T t)
	{
		{ t.create_command_buffer() };
	} && 
	
	requires(T t, QueueOperation operation)
	{
		{ t.get_queue(operation) }			-> std::same_as<queue_handle>;
		{ t.get_resource_handler() };
	};

	template <IsRenderInterface Interface>
	class BaseRenderInterface : public Interface
	{
	public:
		texture_handle DefaultTextureHandle = invalid_handle;
	};
}