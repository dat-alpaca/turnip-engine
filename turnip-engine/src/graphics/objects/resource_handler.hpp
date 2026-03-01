#pragma once
#include <span>

#include "assets/texture/texture_asset.hpp"
#include "graphics/objects/buffer.hpp"
#include "graphics/objects/command_buffer.hpp"
#include "graphics/objects/descriptor.hpp"
#include "graphics/objects/pipeline.hpp"
#include "graphics/objects/render_target.hpp"
#include "graphics/objects/shader.hpp"
#include "graphics/objects/sync.hpp"
#include "graphics/objects/texture.hpp"

#include "core/data-structures/range.hpp"

namespace tur
{
	template <typename T, typename RHI>
	concept IsGraphicsResourceHandler = requires(T t, RHI* rhi)
	{
		{ t.initialize(rhi) };
		{ t.destroy() };
	} && 
	
	requires(T t, const ShaderDescriptor& desc, shader_handle handle)
	{
        { t.create_shader(desc) }									-> std::same_as<shader_handle>;
		{ t.destroy_shader(handle) };
	} && 
	
	requires(T t, const RenderTargetDescriptor& desc, render_target_handle handle)
	{
        { t.create_render_target(desc) }							-> std::same_as<render_target_handle>;
		{ t.destroy_shader(handle) };
	} && 

	requires(T t, const DescriptorSetLayoutDescriptor& desc, descriptor_set_layout_handle handle)
	{
        { t.create_descriptor_set_layout(desc) }					-> std::same_as<descriptor_set_layout_handle>;
		{ t.destroy_descriptor_set_layout(handle) };
	} &&
	requires(T t, descriptor_set_layout_handle layoutHandle)
	{
        { t.create_descriptor_set(layoutHandle) }					-> std::same_as<descriptor_set_handle>;
	} && 
	requires(T t, descriptor_set_handle setHandle, buffer_handle bufferHandle, const Range& range, u32 binding)
	{
		{ t.write_uniform_buffer_to_set(setHandle, bufferHandle, range, binding) };
		{ t.write_storage_buffer_to_set(setHandle, bufferHandle, range, binding) };
	} &&

	requires(T t, const PipelineDescriptor& desc, ComputePipelineDescriptor compDesc, pipeline_handle handle)
	{ 
		{ t.create_graphics_pipeline(desc) }						-> std::same_as<pipeline_handle>;
		{ t.create_compute_pipeline(compDesc) }						-> std::same_as<pipeline_handle>;
		{ t.destroy_graphics_pipeline(handle) };
		{ t.destroy_compute_pipeline(handle) };
	} && 
	
	requires(T t, const BufferDescriptor& desc, const void* data, Range range, u32 size)
	{
        { t.create_buffer(desc, data, range) }						-> std::same_as<buffer_handle>;
        { t.create_empty_buffer(desc, size) }						-> std::same_as<buffer_handle>;
	} &&
	
	requires(T t, buffer_handle handle, const void* data, Range range)
	{
        { t.update_buffer(handle, data, range) };
	} &&

	requires(T t, buffer_handle handle, texture_handle texture, u32 number)
	{
		{ t.copy_buffer(handle, handle, number, number, number) };
		{ t.copy_buffer_to_texture(handle, texture, number, number, number) };
		{ t.destroy_buffer(handle) };
	} && 
	
	requires(T t, const TextureDescriptor& desc, texture_handle handle, const TextureAsset& asset, const glm::vec2& offset, u32 layer)
	{
        { t.create_texture(desc, asset) }							-> std::same_as<texture_handle>;
        { t.create_empty_texture(desc) }							-> std::same_as<texture_handle>;
		{ t.update_texture(handle, asset) };
		{ t.update_array_texture_layer(handle, asset, offset, layer) };
		{ t.destroy_texture(handle) };
	} &&

	requires(T t, fence_handle fence, semaphore_handle semaphore)
	{
        { t.create_fence() }							-> std::same_as<fence_handle>;
        { t.create_semaphore() }						-> std::same_as<semaphore_handle>;
		{ t.destroy_fence(fence) };
		{ t.destroy_semaphore(semaphore) };
	} &&

	requires(T t, command_buffer_handle commandBufferHandle)
	{
        { t.create_primary_command_buffer() }			-> std::same_as<command_buffer_handle>;
	};

	/* Concepts */
	template <typename T, typename DataType>
	concept IsGraphicsGenericResource = requires(T t, buffer_handle handle, std::span<const DataType> data, u32 offset)
	{
		{ t.template update_buffer<const DataType>(handle, data, offset) };
	};

	template <typename T, typename RHI>
	requires IsGraphicsResourceHandler<T, RHI>
	class BaseGraphicsResourceHandler : public T
	{
		template <typename DataType>
		requires IsGraphicsGenericResource<T, const DataType> buffer_handle update_buffer(std::span<const DataType> s)
		{
			return static_cast<T&>(*this).template create_buffer<DataType>(s);
		}
	};
}