#pragma once
#include "common.hpp"
#include "objects/opengl_command_buffer.hpp"
#include "objects/opengl_resource_handler.hpp"

#include "graphics/objects/queue.hpp"
#include "graphics/render_interface.hpp"

#include "config/config_data.hpp"
#include "platform/platform.hpp"

namespace tur::gl
{
	class RenderInterfaceGL
	{
	public:
		void initialize(const ConfigData&, Window& window);
		void shutdown();
		void on_event(Event& event);

	public:
		bool begin_frame();
		void submit([[maybe_unused]] queue_handle);
		void present([[maybe_unused]] queue_handle);
		void end_frame();

	public:
		CommandBuffer create_command_buffer();
		EmptyCommandBuffer get_current_command_buffer() { return {}; }

	public:
		queue_handle get_queue(QueueOperation operation) { return invalid_handle; }
		inline ResourceHandler& get_resource_handler() { return mResources; }

	private:
		NON_OWNING Window* rWindow = nullptr;
		ResourceHandler mResources;
	};

}

namespace tur
{
	using RenderInterface = BaseRenderInterface<gl::RenderInterfaceGL>;
}