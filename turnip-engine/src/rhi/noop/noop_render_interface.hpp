#pragma once
#include "common.hpp"
#include "graphics/objects/queue.hpp"
#include "graphics/render_interface.hpp"

#include "objects/noop_command_buffer.hpp"
#include "objects/noop_resource_handler.hpp"

#include "config/config_data.hpp"
#include "platform/platform.hpp"

namespace tur::noop
{
	class RenderInterfaceNOOP
	{
	public:
		void initialize(const ConfigData& configData, Window& window) {}
		void shutdown() {}
		void on_event(Event& event) {}

	public:
		bool begin_frame() {}
		void submit([[maybe_unused]] queue_handle) {}
		void present([[maybe_unused]] queue_handle) {}
		void end_frame() {}

	public:
		CommandBuffer create_command_buffer() { return CommandBuffer(); }
		EmptyCommandBuffer get_current_command_buffer() { return {}; }

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
	using RenderInterface = BaseRenderInterface<noop::RenderInterfaceNOOP>;
}