#include "gl_render_interface.hpp"
#include "platform/platform.hpp"

namespace tur::gl
{
	void RenderInterfaceGL::initialize(const ConfigData& configData, Window& window)
	{
		rWindow = &window;
		mConfigData = configData;

		mResources.initialize(this);
		glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE);
	}
	void RenderInterfaceGL::shutdown()
	{
		/* Blank */
	}
	void RenderInterfaceGL::on_event(Event& event)
	{
		/* Blank */
	}

	bool RenderInterfaceGL::begin_frame()
	{
		/* Blank */
		return true;
	}
	void RenderInterfaceGL::submit([[maybe_unused]] queue_handle)
	{
		/* Blank */
	}
	void RenderInterfaceGL::present([[maybe_unused]] queue_handle)
	{
		platform::WindowingSystem::get_binder().present();
	}
	void RenderInterfaceGL::end_frame()
	{
		/* Blank */
	}

	CommandBuffer RenderInterfaceGL::create_command_buffer()
	{
		return CommandBuffer(this);
	}
}
