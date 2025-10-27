#pragma once
#include "graphics/objects/descriptor.hpp"
#include "graphics/objects/render_target.hpp"
#include "graphics/objects/resource_handler.hpp"
#include "graphics/objects/texture.hpp"
#include "graphics/render_interface.hpp"
#include "graphics/types/access_flags.hpp"

#include "rhi/opengl/objects/opengl_shader_loader.hpp"

namespace tur::noop
{
	class RenderInterfaceNOOP;

	class ResourceHandlerNOOP
	{
	public:
		void initialize(RenderInterfaceNOOP* rhi) {}
		void destroy() {}

	public:
		shader_handle create_shader(const ShaderDescriptor& descriptor) { return invalid_handle; }
		void destroy_shader(shader_handle shaderHandle) {}

	public:
		render_target_handle create_render_target(const RenderTargetDescriptor& descriptor) { return invalid_handle; }
		void destroy_render_target(render_target_handle renderTargetHandle) {}

	public:
		descriptor_set_layout_handle
		create_descriptor_set_layout(const DescriptorSetLayoutDescriptor& descriptorSetLayout)
		{
			return invalid_handle;
		}
		void destroy_descriptor_set_layout(descriptor_set_layout_handle descriptorSetLayoutHandle) {}

		descriptor_set_handle create_descriptor_set(descriptor_set_layout_handle layoutHandle)
		{
			return invalid_handle;
		}
		void write_uniform_buffer_to_set(
			descriptor_set_handle setHandle, buffer_handle bufferHandle, const Range& range, u32 binding
		)
		{
		}
		void write_texture_to_set(descriptor_set_handle setHandle, texture_handle textureHandle, u32 binding) {}

	public:
		pipeline_handle create_graphics_pipeline(const PipelineDescriptor& descriptor) { return invalid_handle; }
		void destroy_graphics_pipeline(pipeline_handle pipelineHandle) {}

	public:
		buffer_handle create_buffer(const BufferDescriptor& descriptor, const void* data, Range range)
		{
			return invalid_handle;
		}
		buffer_handle create_empty_buffer(const BufferDescriptor& descriptor, u32 size) { return invalid_handle; }
		void update_buffer(buffer_handle bufferHandle, const void* data, Range range) {}
		void copy_buffer(buffer_handle source, buffer_handle destination, u32 size, u32 srcOffset, u32 dstOffset) {}
		void copy_buffer_to_texture(buffer_handle source, texture_handle target, u32 size, u32 srcOffset, u32 dstOffset)
		{
		}
		void destroy_buffer(buffer_handle bufferHandle) {}

	public:
		texture_handle create_texture(const TextureDescriptor& descriptor, const TextureAsset& asset)
		{
			return invalid_handle;
		}
		texture_handle create_empty_texture(const TextureDescriptor& descriptor) { return invalid_handle; }
		void update_texture(texture_handle textureHandle, const TextureAsset& asset) {}
		void destroy_texture(texture_handle textureHandle) {}

	private:
		NON_OWNING RenderInterfaceNOOP* rRHI = nullptr;
		ShaderLoader mShaderLoader;
		vk::Device rDevice;
	};
}

namespace tur
{
	using ResourceHandler = BaseGraphicsResourceHandler<noop::ResourceHandlerNOOP, noop::RenderInterfaceNOOP>;
}