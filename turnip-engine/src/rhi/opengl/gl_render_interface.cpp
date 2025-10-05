#include "gl_render_interface.hpp"

namespace tur::gl
{
	void RenderInterfaceGL::initialize(const ConfigData&, Window& window)
	{
		rWindow = &window;
	}
	void RenderInterfaceGL::shutdown()
	{
		/* Blank */
	}

	void RenderInterfaceGL::begin_frame()
	{
		/* Blank */
	}
	void RenderInterfaceGL::submit()
	{
		/* Blank */
	}
	void RenderInterfaceGL::present(UNUSED queue_handle)
	{
		GraphicsBinder::present();
	}
	void RenderInterfaceGL::end_frame()
	{
		/* Blank */
	}

	CommandBufferGL RenderInterfaceGL::create_command_buffer()
	{
		return CommandBufferGL();
	}
}
