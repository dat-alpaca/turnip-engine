#include "opengl_command_buffer.hpp"

namespace tur::gl
{
	CommandBufferGL::CommandBufferGL(NON_OWNING RenderInterfaceGL* rhi)
	{
		rRHI = rhi;
		glCreateVertexArrays(1, &mVAO);
	}
	CommandBufferGL::CommandBufferGL()
	{
		glCreateVertexArrays(1, &mVAO);
	}

	void CommandBufferGL::reset(EmptyCommandBuffer commandBuffer)
	{
	}
}