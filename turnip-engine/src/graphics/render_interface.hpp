#pragma once
#include "config/config_data.hpp"
#include "core/event/events.hpp"
#include "defines.hpp"
#include "objects/queue.hpp"
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
	
	requires(T t, queue_handle queue)
	{
		{ t.begin_frame() }			-> std::same_as<bool>;
		{ t.submit(queue) };
		{ t.present(queue) };
		{ t.end_frame() };
	} &&

	requires(T t)
	{
		{ t.create_command_buffer() };
		{ t.get_current_command_buffer() };
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