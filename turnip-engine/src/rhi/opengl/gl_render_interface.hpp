#pragma once
#include "defines.hpp"
#include "graphics/objects/command_buffer.hpp"
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
		void initialize(const ConfigData& configData, Window& window);
		void shutdown();
		void on_event(Event& event);

	public:
		void begin_frame([[maybe_unused]] fence_handle inFlightFence = invalid_handle, [[maybe_unused]] u32 inFlightTimeout = 0);
		std::optional<image_handle> acquire_swapchain_image([[maybe_unused]] semaphore_handle waitFor, [[maybe_unused]] u32 timeout = 0);
		void reset_fence([[maybe_unused]] fence_handle inFlightFence);

		void submit([[maybe_unused]] queue_handle = invalid_handle, [[maybe_unused]] command_buffer_handle commandBuffer = invalid_handle, [[maybe_unused]] fence_handle inFlightFence = invalid_handle, [[maybe_unused]] semaphore_handle waitFor = invalid_handle, [[maybe_unused]] semaphore_handle signal = invalid_handle);
		void present([[maybe_unused]] queue_handle = invalid_handle, [[maybe_unused]] semaphore_handle waitFor = invalid_handle, [[maybe_unused]] u32 imageHandle = invalid_handle);
		void end_frame();

	public:
		CommandBuffer create_command_buffer();

	public:
		queue_handle get_queue(QueueOperation operation) { return invalid_handle; }
		inline ResourceHandler& get_resource_handler() { return mResources; }

	public:
		const ConfigData& get_config_data() const { return mConfigData; }

	private:
		NON_OWNING Window* rWindow = nullptr;
		ResourceHandler mResources;
		ConfigData mConfigData;
	};
}

namespace tur
{
	using RenderInterface = BaseRenderInterface<gl::RenderInterfaceGL>;
}