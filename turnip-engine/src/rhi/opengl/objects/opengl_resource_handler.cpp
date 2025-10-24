#include "opengl_resource_handler.hpp"
#include "rhi/opengl/allocators/allocators.hpp"
#include <glad/gl.h>

namespace tur::gl
{
	void ResourceHandler::initialize(RenderInterfaceGL* rhi)
	{
		TUR_ASSERT(rhi, "Null RHI passed to ResourceHandler");
		rRHI = rhi;
	}
	void ResourceHandler::destroy()
	{
		/* Blank */
		// TODO: destroy all objects
	}

	shader_handle ResourceHandler::create_shader(const ShaderDescriptor& descriptor)
	{
		return mShaders.add(allocate_shader(descriptor));
	}
	void ResourceHandler::destroy_shader(shader_handle shaderHandle)
	{
		glDeleteShader(mShaders.get(shaderHandle).handle);
		mShaders.remove(shaderHandle);
	}

	render_target_handle ResourceHandler::create_render_target(const RenderTargetDescriptor& descriptor)
	{
		RenderTarget renderTarget = allocate_render_target(descriptor);

		u32 index = 0;
		u32 width, height;
		for (const auto& attachment : descriptor.attachmentDescriptions)
		{
			// TODO: check if all attachments are the same size.

			attachment.loadOp;  // UNUSED
			attachment.storeOp; // UNUSED

			width = attachment.textureDescriptor.width;
			height = attachment.textureDescriptor.height;

			texture_handle attachTexture = create_empty_texture(attachment.textureDescriptor);
			gl_handle glAttachTextureHandle = mTextures.get(attachTexture).handle;

			glNamedFramebufferTexture(renderTarget.handle, GL_COLOR_ATTACHMENT0 + index, glAttachTextureHandle, 0);

			renderTarget.colorAttachments.push_back(attachTexture);
			++index;
		}

		u32 rbo;
		glCreateRenderbuffers(1, &rbo);
		glNamedRenderbufferStorage(rbo, GL_DEPTH24_STENCIL8, width, height);
		glNamedFramebufferRenderbuffer(renderTarget.handle, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		if (glCheckNamedFramebufferStatus(renderTarget.handle, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			TUR_LOG_CRITICAL("Incomplete framebuffer: ", renderTarget.handle);

		return mRenderTargets.add(renderTarget);
	}
	void ResourceHandler::destroy_render_target(render_target_handle renderTargetHandle)
	{
		RenderTarget renderTarget = mRenderTargets.get(renderTargetHandle);
		for (const auto& textureHandle : renderTarget.colorAttachments)
			destroy_texture(textureHandle);

		glDeleteFramebuffers(1, &renderTarget.handle);
	}

	descriptor_set_layout_handle
	ResourceHandler::create_descriptor_set_layout(const DescriptorSetLayoutDescriptor& descriptorSetLayout)
	{
		return mSetLayouts.add({descriptorSetLayout});
	}
	void ResourceHandler::destroy_descriptor_set_layout(descriptor_set_layout_handle descriptorSetLayoutHandle)
	{
		mSetLayouts.remove(descriptorSetLayoutHandle);
	}

	// TODO: write_resource_to_set()
	descriptor_set_handle ResourceHandler::create_descriptor_set(descriptor_set_layout_handle layoutHandle)
	{
		DescriptorSet set;
		set.layout = layoutHandle;

		return mDescriptorSets.add(set);
	}
	void ResourceHandler::write_uniform_buffer_to_set(
		descriptor_set_handle setHandle, buffer_handle bufferHandle, const Range& range, u32 binding
	)
	{
		// UNUSED: range
		DescriptorSet& set = mDescriptorSets.get(setHandle);
		const Buffer& buffer = mBuffers.get(bufferHandle);

		set.descriptors[binding] = {
			.resourceHandle = buffer.handle, .type = DescriptorType::UNIFORM_BUFFER, .binding = binding};
	}
	void
	ResourceHandler::write_texture_to_set(descriptor_set_handle setHandle, texture_handle textureHandle, u32 binding)
	{
		DescriptorSet& set = mDescriptorSets.get(setHandle);
		const Texture& texture = mTextures.get(textureHandle);

		set.descriptors[binding] = {
			.resourceHandle = texture.handle, .type = DescriptorType::COMBINED_IMAGE_SAMPLER, .binding = binding};
	}

	pipeline_handle ResourceHandler::create_graphics_pipeline(const PipelineDescriptor& descriptor)
	{
		gl_handle pipelineID = glCreateProgram();

		// Shaders:
		const shader_handle& vertexShader = descriptor.vertexShader;
		const shader_handle& tesselationControlShader = descriptor.tesselationControlShader;
		const shader_handle& tesselationEvaluationShader = descriptor.tesselationEvaluationShader;
		const shader_handle& geometryShader = descriptor.geometryShader;
		const shader_handle& fragmentShader = descriptor.fragmentShader;

		{
			if (vertexShader == invalid_handle)
				TUR_LOG_CRITICAL("Vertex shader not specified in a graphics pipeline");

			glAttachShader(pipelineID, mShaders.get(vertexShader).handle);

			if (tesselationControlShader != invalid_handle)
				glAttachShader(pipelineID, mShaders.get(tesselationControlShader).handle);

			if (tesselationEvaluationShader != invalid_handle)
				glAttachShader(pipelineID, mShaders.get(tesselationEvaluationShader).handle);

			if (geometryShader != invalid_handle)
				glAttachShader(pipelineID, mShaders.get(geometryShader).handle);

			if (fragmentShader == invalid_handle)
				TUR_LOG_CRITICAL("Fragment shader not specified in a graphics pipeline");

			glAttachShader(pipelineID, mShaders.get(fragmentShader).handle);
		}

		glLinkProgram(pipelineID);
		glValidateProgram(pipelineID);

		check_program_link_errors(pipelineID);

		// Shader destruction:
		{
			destroy_shader(vertexShader);

			if (tesselationControlShader != invalid_handle)
				destroy_shader(tesselationControlShader);

			if (tesselationEvaluationShader != invalid_handle)
				destroy_shader(tesselationEvaluationShader);

			if (geometryShader != invalid_handle)
				destroy_shader(geometryShader);

			destroy_shader(fragmentShader);
		}

		Pipeline pipeline;
		{
			pipeline.inputAssemblyStage = descriptor.inputAssemblyStage;
			pipeline.vertexInputStage = descriptor.vertexInputStage;

			pipeline.rasterizerStage = descriptor.rasterizerStage;
			pipeline.setLayout = descriptor.setLayout;
			pipeline.handle = pipelineID;
		}

		return mPipelines.add(pipeline);
	}
	void ResourceHandler::destroy_graphics_pipeline(pipeline_handle pipelineHandle)
	{
		const auto& pipeline = mPipelines.remove(pipelineHandle);
		glDeleteProgram(pipeline.handle);
	}

	buffer_handle ResourceHandler::create_buffer(const BufferDescriptor& descriptor, const void* data, Range range)
	{
		std::vector<byte> convertedData(range.size);
		if (data)
			std::copy((u8*)data + range.offset, (u8*)data + range.size, convertedData.begin());

		return mBuffers.add(allocate_buffer(descriptor, convertedData));
	}
	buffer_handle ResourceHandler::create_empty_buffer(const BufferDescriptor& descriptor, u32 size)
	{
		return create_buffer(descriptor, nullptr, {.size = size});
	}
	void ResourceHandler::update_buffer(buffer_handle bufferHandle, const void* data, Range range)
	{
		const auto& buffer = mBuffers.get(bufferHandle);
		glNamedBufferSubData(buffer.handle, range.offset, range.size, data);
	}
	void ResourceHandler::copy_buffer(
		buffer_handle source, buffer_handle destination, u32 size, u32 srcOffset, u32 dstOffset
	)
	{
		auto& srcBuffer = mBuffers.get(source);
		auto& dstBuffer = mBuffers.get(destination);

		glCopyNamedBufferSubData(srcBuffer.handle, dstBuffer.handle, srcOffset, dstOffset, size);
	}
	void ResourceHandler::copy_buffer_to_texture(
		buffer_handle source, texture_handle destination, u32 size, u32 srcOffset, u32 dstOffset
	)
	{
		TUR_LOG_WARN("Unused function: copy_buffer_to_texture");
	}
	void ResourceHandler::destroy_buffer(buffer_handle bufferHandle)
	{
		const auto& buffer = mBuffers.remove(bufferHandle);
		glDeleteBuffers(1, &buffer.handle);
	}

	texture_handle ResourceHandler::create_texture(const TextureDescriptor& descriptor, const TextureAsset& asset)
	{
		texture_handle textureHandle = create_empty_texture(descriptor);
		update_texture(textureHandle, asset);

		return textureHandle;
	}
	texture_handle ResourceHandler::create_empty_texture(const TextureDescriptor& descriptor)
	{
		return mTextures.add(allocate_texture(descriptor));
	}
	void ResourceHandler::update_texture(texture_handle textureHandle, const TextureAsset& asset)
	{
		Texture& texture = mTextures.get(textureHandle);

		gl_handle dataFormat = get_texture_data_format(asset.dataFormat);
		gl_handle dataType = !asset.floatTexture ? GL_UNSIGNED_BYTE : GL_FLOAT;

		switch (texture.descriptor.type)
		{
			case TextureType::TEXTURE_2D:
				glTextureSubImage2D(
					texture.handle, 0, 0, 0, asset.width, asset.height, dataFormat, dataType, asset.data.data()
				);
				break;

			case TextureType::CUBE_MAP:
			case TextureType::ARRAY_TEXTURE_2D:
			case TextureType::TEXTURE_3D:
				glTextureSubImage3D(
					texture.handle, 0, 0, 0, 0, asset.width, asset.height, 1, dataFormat, dataType, asset.data.data()
				);
				break;

			default:
				TUR_LOG_ERROR("Invalid texture descriptor type on update");
		}
	}
	void ResourceHandler::destroy_texture(texture_handle textureHandle)
	{
		const auto& texture = mTextures.remove(textureHandle);
		glDeleteTextures(1, &texture.handle);
	}
}