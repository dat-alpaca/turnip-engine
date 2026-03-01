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

	void RenderInterfaceGL::begin_frame(fence_handle, u32)
	{
		/* Blank */
	}
	std::optional<image_handle> RenderInterfaceGL::acquire_swapchain_image(semaphore_handle, u32)
	{
		return 0;
	}
	void RenderInterfaceGL::submit(queue_handle, command_buffer_handle, fence_handle, semaphore_handle, semaphore_handle)
	{
		/* Blank */
	}
	void RenderInterfaceGL::reset_fence(fence_handle)
	{
		/* Blank */
	}
	void RenderInterfaceGL::present(queue_handle, semaphore_handle, u32)
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
