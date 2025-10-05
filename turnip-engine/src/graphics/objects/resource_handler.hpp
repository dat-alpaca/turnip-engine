#pragma once
#include "assets/texture_asset.hpp"
#include "graphics/objects/buffer.hpp"
#include "graphics/objects/pipeline.hpp"
#include "graphics/objects/render_target.hpp"
#include "graphics/objects/shader.hpp"
#include "graphics/objects/texture.hpp"

namespace tur
{
	// clang-format off
	template <typename T, typename RHI>
	concept IsGraphicsResourceHandler = requires(T t, RHI* rhi)
	{
		{ t.initialize(rhi) };
		{ t.destroy() };
	} && requires(T t, const ShaderDescriptor& desc, shader_handle handle)
	{
        { t.create_shader(desc) }									-> std::same_as<shader_handle>;
		{ t.destroy_shader(handle) };
	} && requires(T t, const RenderTargetDescriptor& desc, render_target_handle handle)
	{
        { t.create_render_target(desc) }							-> std::same_as<render_target_handle>;
		{ t.destroy_shader(handle) };
	} && requires(T t, const PipelineDescriptor& desc, pipeline_handle handle)
	{ 
		{ t.create_graphics_pipeline(desc) }						-> std::same_as<pipeline_handle>;
	} && requires(T t, const BufferDescriptor& desc, texture_handle texture, buffer_handle handle, const std::vector<byte>& data, u32 number)
	{
        { t.create_buffer(desc, data) }								-> std::same_as<buffer_handle>;
        { t.create_empty_buffer(desc, number) }						-> std::same_as<buffer_handle>;
		{ t.update_buffer(handle, data, number) };
		{ t.copy_buffer(handle, handle, number, number, number) };
		{ t.copy_buffer_to_texture(handle, texture, number, number, number) };
		{ t.destroy_buffer(handle) };
	} && requires(T t, const TextureDescriptor& desc, texture_handle handle, const TextureAsset& asset)
	{
        { t.create_texture(desc, asset) }							-> std::same_as<texture_handle>;
        { t.create_empty_texture(desc) }							-> std::same_as<texture_handle>;
		{ t.update_texture(handle, asset) };
		{ t.destroy_texture(handle) };
	};
	// clang-format on

	template <typename T, typename RHI>
	requires IsGraphicsResourceHandler<T, RHI>
	class BaseGraphicsResourceHandler : public T
	{
	};
}