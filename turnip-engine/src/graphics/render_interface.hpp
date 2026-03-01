#pragma once
#include "config/config_data.hpp"
#include "core/event/events.hpp"

#include "defines.hpp"

#include "objects/sync.hpp"
#include "objects/queue.hpp"
#include "objects/image.hpp"
#include "objects/command_buffer.hpp"

#include "platform/platform.hpp"
#include "types/queue_operations.hpp"

namespace tur
{
	template <typename T>
	concept IsRenderInterface = requires(T t, const ConfigData& configData, Window& window, Event& event)
	{
		{ t.initialize(configData, window) };
		{ t.shutdown() };
		{ t.on_event(event) };
	} && 
	
	requires(T t, queue_handle queue, u32 timeout, fence_handle inFlightFence, semaphore_handle waitFor, semaphore_handle signal, command_buffer_handle cb)
	{
		{ t.begin_frame(inFlightFence, timeout) };
		{ t.acquire_swapchain_image(signal, timeout) } 		-> std::same_as<std::optional<image_handle>>;
		{ t.reset_fence(inFlightFence) };
		
		{ t.submit(queue, cb, inFlightFence, waitFor, signal) };
		{ t.present(queue, waitFor, timeout /* imageHandle */) };
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