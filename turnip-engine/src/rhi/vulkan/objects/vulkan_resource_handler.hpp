#pragma once
#include "core/assets/texture_asset.hpp"

#include "graphics/objects/descriptor.hpp"
#include "graphics/objects/render_target.hpp"
#include "graphics/objects/resource_handler.hpp"
#include "graphics/objects/texture.hpp"
#include "graphics/render_interface.hpp"
#include "graphics/types/access_flags.hpp"

#include "data-structures/free_list.hpp"
#include "rhi/vulkan/allocators/allocators.hpp"
#include "rhi/vulkan/objects/pipeline.hpp"
#include "rhi/vulkan/objects/render_target.hpp"

#include <span>
#include <unordered_map>
#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	class RenderInterfaceVulkan;

	class ResourceHandler
	{
	public:
		void initialize(RenderInterfaceVulkan* rhi);
		void destroy();

	public:
		shader_handle create_shader(const ShaderDescriptor& descriptor);
		void destroy_shader(shader_handle shaderHandle);

	public:
		render_target_handle create_render_target(const RenderTargetDescriptor& descriptor);
		void destroy_render_target(render_target_handle renderTargetHandle);

	public:
		descriptor_set_layout_handle create_descriptor_set_layout(const DescriptorSetLayout& descriptorSetLayout);
		void destroy_descriptor_set_layout(descriptor_set_layout_handle descriptorSetLayoutHandle);

	public:
		pipeline_handle create_graphics_pipeline(const PipelineDescriptor& descriptor);
		void destroy_graphics_pipeline(pipeline_handle pipelineHandle);

	public:
		buffer_handle create_buffer(const BufferDescriptor& descriptor, const void* data, Range range);
		buffer_handle create_empty_buffer(const BufferDescriptor& descriptor, u32 size);
		void update_buffer(buffer_handle bufferHandle, const void* data, Range range);
		void* map_buffer(buffer_handle bufferHandle, u32 offset, u32 length, AccessFlags flags) { return nullptr; }
		void copy_buffer(buffer_handle source, buffer_handle destination, u32 size, u32 srcOffset, u32 dstOffset);
		void
		copy_buffer_to_texture(buffer_handle source, texture_handle destination, u32 width, u32 height, u32 depth = 1);
		void destroy_buffer(buffer_handle bufferHandle);

	public:
		texture_handle create_texture(const TextureDescriptor& descriptor, const TextureAsset& asset);
		texture_handle create_empty_texture(const TextureDescriptor& descriptor);
		void update_texture(texture_handle textureHandle, const TextureAsset& asset);
		void destroy_texture(texture_handle textureHandle);

	public:
		free_list<vk::ShaderModule>& get_shaders() { return mShaders; }
		free_list<DescriptorSet>& get_set_layouts() { return mSetLayouts; }
		free_list<Buffer>& get_buffers() { return mBuffers; }
		free_list<Pipeline>& get_pipelines() { return mPipelines; }
		free_list<Texture>& get_textures() { return mTextures; }
		free_list<RenderTarget>& get_render_targets() { return mRenderTargets; }

	private:
		NON_OWNING RenderInterfaceVulkan* rRHI = nullptr;
		vk::Device rDevice;

	private:
		free_list<vk::ShaderModule> mShaders;
		free_list<DescriptorSet> mSetLayouts;
		free_list<Buffer> mBuffers;
		free_list<Pipeline> mPipelines;
		free_list<Texture> mTextures;
		free_list<RenderTarget> mRenderTargets;
	};
}

namespace tur
{
	using ResourceHandler = BaseGraphicsResourceHandler<vulkan::ResourceHandler, vulkan::RenderInterfaceVulkan>;
}