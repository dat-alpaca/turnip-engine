#pragma once
#include "assets/texture/texture_asset.hpp"

#include "graphics/objects/descriptor.hpp"
#include "graphics/objects/render_target.hpp"
#include "graphics/objects/resource_handler.hpp"
#include "graphics/objects/texture.hpp"
#include "graphics/render_interface.hpp"
#include "graphics/types/access_flags.hpp"

#include "buffer.hpp"
#include "descriptor.hpp"
#include "pipeline.hpp"
#include "render_target.hpp"
#include "shader.hpp"
#include "texture.hpp"

#include "data-structures/free_list.hpp"
#include "rhi/opengl/objects/opengl_shader_loader.hpp"

#include <glad/gl.h>
#include <span>
#include <unordered_map>

namespace tur::gl
{
	class RenderInterfaceGL;

	class ResourceHandler
	{
	public:
		void initialize(RenderInterfaceGL* rhi);
		void destroy();

	public:
		shader_handle create_shader(const ShaderDescriptor& descriptor);
		void destroy_shader(shader_handle shaderHandle);

	public:
		render_target_handle create_render_target(const RenderTargetDescriptor& descriptor);
		void destroy_render_target(render_target_handle renderTargetHandle);

	public:
		descriptor_set_layout_handle
		create_descriptor_set_layout(const DescriptorSetLayoutDescriptor& descriptorSetLayout);
		void destroy_descriptor_set_layout(descriptor_set_layout_handle descriptorSetLayoutHandle);

		// TODO: write_resource_to_set()
		descriptor_set_handle create_descriptor_set(descriptor_set_layout_handle layoutHandle);
		void write_uniform_buffer_to_set(
			descriptor_set_handle setHandle, buffer_handle bufferHandle, const Range& range, u32 binding
		);
		void write_storage_buffer_to_set(
			descriptor_set_handle setHandle, buffer_handle bufferHandle, const Range& range, u32 binding
		);
		void write_texture_to_set(descriptor_set_handle setHandle, texture_handle textureHandle, u32 binding);

	public:
		pipeline_handle create_graphics_pipeline(const PipelineDescriptor& descriptor);
		void destroy_graphics_pipeline(pipeline_handle pipelineHandle);

	public:
		buffer_handle create_buffer(const BufferDescriptor& descriptor, const void* data, Range range);
		buffer_handle create_empty_buffer(const BufferDescriptor& descriptor, u32 size);
		void update_buffer(buffer_handle bufferHandle, const void* data, Range range);
		void copy_buffer(buffer_handle source, buffer_handle destination, u32 size, u32 srcOffset, u32 dstOffset);
		void
		copy_buffer_to_texture(buffer_handle source, texture_handle target, u32 size, u32 srcOffset, u32 dstOffset);
		void destroy_buffer(buffer_handle bufferHandle);

	public:
		texture_handle create_texture(const TextureDescriptor& descriptor, const TextureAsset& asset);
		texture_handle create_empty_texture(const TextureDescriptor& descriptor);
		void update_texture(texture_handle textureHandle, const TextureAsset& asset);
		void update_array_texture_layer(texture_handle textureHandle, const TextureAsset& asset, const glm::vec2& offset, u32 layer);
		void destroy_texture(texture_handle textureHandle);

	public:
		free_list<Shader>& get_shaders() { return mShaders; }
		free_list<Buffer>& get_buffers() { return mBuffers; }
		free_list<Pipeline>& get_pipelines() { return mPipelines; }
		free_list<Texture>& get_textures() { return mTextures; }
		free_list<RenderTarget>& get_render_targets() { return mRenderTargets; }
		free_list<DescriptorSetLayout>& get_set_layouts() { return mSetLayouts; }
		free_list<DescriptorSet>& get_descriptor_sets() { return mDescriptorSets; }

	private:
		NON_OWNING RenderInterfaceGL* rRHI = nullptr;
		ShaderLoader mShaderLoader;

	private:
		free_list<Shader> mShaders;
		free_list<Buffer> mBuffers;
		free_list<Pipeline> mPipelines;
		free_list<Texture> mTextures;
		free_list<RenderTarget> mRenderTargets;
		free_list<DescriptorSetLayout> mSetLayouts;
		free_list<DescriptorSet> mDescriptorSets;
	};
}

namespace tur
{
	using ResourceHandler = BaseGraphicsResourceHandler<gl::ResourceHandler, gl::RenderInterfaceGL>;
}