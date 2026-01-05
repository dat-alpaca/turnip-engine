#include "opengl_command_buffer.hpp"
#include "logging/logging.hpp"
#include "rhi/opengl/gl_render_interface.hpp"
#include <glad/gl.h>

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
		/* Blank */
	}
	void CommandBufferGL::execute_secondary_buffers(std::span<EmptyCommandBuffer>)
	{
		/* Blank */
	}
	void CommandBufferGL::initialize_secondary()
	{
		/* Blank */
	}

	void CommandBufferGL::begin(render_target_handle renderTarget)
	{
		glBindVertexArray(mVAO);
		auto& resources = rRHI->get_resource_handler();

		if (renderTarget == invalid_handle)
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		else
			glBindFramebuffer(GL_FRAMEBUFFER, resources.get_render_targets().get(renderTarget).handle);

		glDepthRange(0.0, 1.0);
	}
	void CommandBufferGL::begin_rendering()
	{
		clear();
	}
	void CommandBufferGL::end_rendering()
	{
		/* Blank */
	}	
	void CommandBufferGL::end()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void CommandBufferGL::blit()
	{
		/* Blank */
	}

	void CommandBufferGL::clear()
	{
		if (static_cast<u32>(mClearFlags) & static_cast<u32>(ClearFlags::COLOR))
			glClearColor(mColor.color.r, mColor.color.g, mColor.color.b, mColor.color.a);

		if (static_cast<u32>(mClearFlags) & static_cast<u32>(ClearFlags::DEPTH))
			glClearDepth(mColor.depth);

		if (static_cast<u32>(mClearFlags) & static_cast<u32>(ClearFlags::STENCIL))
			glClearStencil(mColor.stencil);

		glClear(mRawClearFlags);
	}
	void CommandBufferGL::set_viewport(const Viewport& viewport)
	{
		const auto x = static_cast<i32>(viewport.x);
		const auto y = static_cast<i32>(viewport.y);
		const auto w = static_cast<i32>(viewport.width);
		const auto h = static_cast<i32>(viewport.height);

		glViewport(x, y, w, h);
	}
	void CommandBufferGL::set_scissor(const Extent2D& scissor)
	{
		const auto x = static_cast<i32>(scissor.x);
		const auto y = static_cast<i32>(scissor.y);
		const auto w = static_cast<i32>(scissor.width);
		const auto h = static_cast<i32>(scissor.height);

		glScissor(x, y, w, h);
	}
	void CommandBufferGL::set_clear_color(const ClearColor& clearColor, ClearFlags flags)
	{
		mColor = clearColor;
		mClearFlags = flags;

		if (static_cast<u32>(mClearFlags) & static_cast<u32>(ClearFlags::COLOR))
			mRawClearFlags |= GL_COLOR_BUFFER_BIT;

		if (static_cast<u32>(mClearFlags) & static_cast<u32>(ClearFlags::DEPTH))
			mRawClearFlags |= GL_DEPTH_BUFFER_BIT;

		if (static_cast<u32>(mClearFlags) & static_cast<u32>(ClearFlags::STENCIL))
			mRawClearFlags |= GL_STENCIL_BUFFER_BIT;
	}

	void CommandBufferGL::bind_vertex_buffer(buffer_handle bufferHandle, u32 binding)
	{
		auto& resources = rRHI->get_resource_handler();
		const Buffer& buffer = resources.get_buffers().get(bufferHandle);
		glVertexArrayVertexBuffer(mVAO, binding, buffer.handle, 0, mCurrentStride);
	}
	void CommandBufferGL::bind_index_buffer(buffer_handle bufferHandle, BufferIndexType indexType)
	{
		mBufferIndexType = indexType;

		auto& resources = rRHI->get_resource_handler();
		const Buffer& buffer = resources.get_buffers().get(bufferHandle);
		glVertexArrayElementBuffer(mVAO, buffer.handle);
	}
	void CommandBufferGL::bind_pipeline(pipeline_handle pipelineHandle)
	{
		auto& resources = rRHI->get_resource_handler();
		mPipeline = resources.get_pipelines().get(pipelineHandle);

		glUseProgram(mPipeline.handle);

		glFrontFace(get_front_face(mPipeline.rasterizerStage.frontFace));
		glPolygonMode(GL_FRONT_AND_BACK, get_polygon_mode(mPipeline.rasterizerStage.polygonMode));
		glCullFace(get_cull_mode(mPipeline.rasterizerStage.cullMode));

		// Setup vertex input bindings & attributes:
		for (const auto& binding : mPipeline.vertexInputStage.vertexBindings)
		{
			mCurrentStride = binding.stride;

			for (const auto& attribute : mPipeline.vertexInputStage.attributes)
			{
				if (attribute.binding != binding.binding)
					continue;

				glEnableVertexArrayAttrib(mVAO, attribute.location);

				glVertexArrayAttribFormat(
					mVAO, attribute.location, static_cast<i32>(get_attribute_format_size(attribute.format)),
					get_attribute_format(attribute.format), false, attribute.offset
				);

				glVertexArrayAttribBinding(mVAO, attribute.location, binding.binding);
			}

			u32 divisor = binding.inputRate == InputRate::VERTEX ? 0 : 1;
			glVertexArrayBindingDivisor(mVAO, binding.binding, divisor);
		}
	}
	void CommandBufferGL::bind_descriptor_set(descriptor_set_handle setHandle)
	{
		auto& resources = rRHI->get_resource_handler();
		DescriptorSet set = resources.get_descriptor_sets().get(setHandle);

		// Setup uniforms:
		for (const auto& [binding, descriptor] : set.descriptors)
		{
			switch (descriptor.type)
			{
				case DescriptorType::UNIFORM_BUFFER:
				{
					glBindBuffer(GL_UNIFORM_BUFFER, descriptor.resourceHandle);
					glBindBufferBase(GL_UNIFORM_BUFFER, binding, descriptor.resourceHandle);
				}
				break;

				case DescriptorType::STORAGE_BUFFER:
				{
					glBindBuffer(GL_SHADER_STORAGE_BUFFER, descriptor.resourceHandle);
					glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, descriptor.resourceHandle);
				}
				break;

				case DescriptorType::COMBINED_IMAGE_SAMPLER:
				{
					glBindTextureUnit(binding, descriptor.resourceHandle);
				}
				break;

				default:
					TUR_LOG_CRITICAL("Invalid descriptor type passed.");
			}
		}
	}

	void CommandBufferGL::draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance)
	{
		gl_handle topology = get_primitive_topology(mPipeline.inputAssemblyStage.topology);

		if (instanceCount == 1)
			glDrawArrays(topology, firstVertex, vertexCount);
		else
			glDrawArraysInstanced(topology, firstVertex, vertexCount, instanceCount);
	}
	void CommandBufferGL::draw_indexed(u32 indexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance)
	{
		gl_handle topology = get_primitive_topology(mPipeline.inputAssemblyStage.topology);

		if (instanceCount == 1)
			glDrawElements(topology, indexCount, get_buffer_index_type(mBufferIndexType), nullptr);
		else
			glDrawElementsInstanced(
				topology, indexCount, get_buffer_index_type(mBufferIndexType), nullptr, instanceCount
			);
	}
}