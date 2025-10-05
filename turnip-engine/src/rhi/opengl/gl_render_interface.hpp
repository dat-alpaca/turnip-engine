#pragma once
#include "common.hpp"
#include "objects/opengl_command_buffer.hpp"

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

	public:
		void begin_frame();
		void submit();
		void present(UNUSED queue_handle);
		void end_frame();

	public:
		CommandBufferGL create_command_buffer();
		EmptyCommandBuffer get_current_command_buffer() { return {}; }

	public:
		auto get_resource_handler();
		auto get_queue(QueueOperation operation);

	private:
		NON_OWNING Window* rWindow = nullptr;
	};

}

namespace tur
{
	using RenderInterface = BaseRenderInterface<gl::RenderInterfaceGL>;
}