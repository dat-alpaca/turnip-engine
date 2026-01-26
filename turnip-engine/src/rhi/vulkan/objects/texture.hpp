#pragma once
#include "graphics/objects/texture.hpp"
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	struct Texture
	{
		TextureDescriptor descriptor;

		vk::Image image;
		vk::ImageView imageView;
		vk::Sampler sampler;
		VmaAllocation allocation = nullptr;

		vk::Extent3D extent;
		vk::Format format = vk::Format::eR8G8B8A8Unorm;
		vk::ImageLayout layout = vk::ImageLayout::eUndefined;
	};
}

namespace tur::vulkan
{
	constexpr static inline vk::ImageType get_texture_type(TextureType type)
	{
		switch (type)
		{
			case TextureType::ARRAY_TEXTURE_2D:
			case TextureType::TEXTURE_2D:
				return vk::ImageType::e2D;

			case TextureType::TEXTURE_3D:
				return vk::ImageType::e3D;

			case TextureType::CUBE_MAP:
				return vk::ImageType::e2D;
		}

		TUR_LOG_ERROR("Invalid Texture Type: {}. Default: e2D", static_cast<int>(type));
		return vk::ImageType::e2D;
	}
	constexpr static inline vk::ImageViewType get_texture_view_type(TextureType type)
	{
		switch (type)
		{
			case TextureType::ARRAY_TEXTURE_2D:
				return vk::ImageViewType::e2DArray;

			case TextureType::TEXTURE_2D:
				return vk::ImageViewType::e2D;

			case TextureType::TEXTURE_3D:
				return vk::ImageViewType::e3D;

			case TextureType::CUBE_MAP:
				return vk::ImageViewType::eCube;
		}

		TUR_LOG_ERROR("Invalid Image View Type: {}. Default: e2D", static_cast<int>(type));
		return vk::ImageViewType::e2D;
	}

	constexpr static inline vk::SamplerAddressMode get_wrap_mode(WrapMode mode)
	{
		switch (mode)
		{
			case WrapMode::REPEAT:
				return vk::SamplerAddressMode::eRepeat;

			case WrapMode::MIRRORED_REPEAT:
				return vk::SamplerAddressMode::eMirroredRepeat;

			case WrapMode::CLAMP_TO_EDGE:
				return vk::SamplerAddressMode::eClampToEdge;

			case WrapMode::CLAMP_TO_BORDER:
				return vk::SamplerAddressMode::eClampToBorder;
		}

		TUR_LOG_ERROR("Invalid Texture Wrap Mode: {}. Default: eRepeat", static_cast<int>(mode));
		return vk::SamplerAddressMode::eRepeat;
	}

	struct FilterData
	{
		vk::Filter magFilter;
		vk::Filter minFilter;
		vk::SamplerMipmapMode mipmapMode;
	};

	constexpr static inline vk::Filter get_filter_mode(FilterMode mode, bool isMinFilter)
	{
		switch (mode)
		{
			case FilterMode::NEAREST:
				return vk::Filter::eNearest;

			case FilterMode::LINEAR:
			case FilterMode::BILINEAR:
			case FilterMode::TRILINEAR:
				return vk::Filter::eLinear;
		}

		TUR_LOG_ERROR("Invalid Texture Wrap Mode: {}. Default: eNearest", static_cast<int>(mode));
		return vk::Filter::eNearest;
	}

	constexpr static inline vk::SamplerMipmapMode get_mipmap_mode(FilterMode minFilter)
	{
		switch (minFilter)
		{
			case FilterMode::NEAREST:
				return vk::SamplerMipmapMode::eNearest;

			case FilterMode::LINEAR:
			case FilterMode::BILINEAR:
			case FilterMode::TRILINEAR:
				return vk::SamplerMipmapMode::eLinear;
		}

		TUR_LOG_ERROR("Invalid Texture Wrap Mode: {}. Default: eNearest", static_cast<int>(minFilter));
		return vk::SamplerMipmapMode::eNearest;
	}

	constexpr static inline vk::Format get_texture_format(TextureFormat format)
	{
		switch (format)
		{
			case TextureFormat::DEPTH_16_UNORM:
				return vk::Format::eD16Unorm;

			case TextureFormat::DEPTH_STENCIL24_S8U_INT:
				return vk::Format::eD24UnormS8Uint;

			case TextureFormat::R8_UNORM:
				return vk::Format::eR8Unorm;

			case TextureFormat::RG8_UNORM:
				return vk::Format::eR8G8Unorm;

			case TextureFormat::RGB8_UNORM:
				return vk::Format::eR8G8B8Unorm;

			case TextureFormat::RGBA8_UNORM:
				return vk::Format::eR8G8B8A8Unorm;

			case TextureFormat::RGBA16_SFLOAT:
				return vk::Format::eR16G16B16A16Sfloat;

			case TextureFormat::B8G8R8A8_UNORM:
				return vk::Format::eB8G8R8A8Unorm;
		}

		TUR_LOG_ERROR("Invalid Texture Format: {}. Default: RGBA (32-bit)", static_cast<int>(format));
		return vk::Format::eR8G8B8A8Unorm;
	}

	constexpr static inline TextureFormat get_texture_format(vk::Format format)
	{
		switch (format)
		{
			case vk::Format::eD16Unorm:
				return TextureFormat::DEPTH_16_UNORM;

			case vk::Format::eD24UnormS8Uint:
				return TextureFormat::DEPTH_STENCIL24_S8U_INT;

			case vk::Format::eR8Unorm:
				return TextureFormat::R8_UNORM;

			case vk::Format::eR8G8Unorm:
				return TextureFormat::RG8_UNORM;

			case vk::Format::eR8G8B8Unorm:
				return TextureFormat::RGB8_UNORM;

			case vk::Format::eR8G8B8A8Unorm:
				return TextureFormat::RGBA8_UNORM;

			case vk::Format::eR16G16B16A16Sfloat:
				return TextureFormat::RGBA16_SFLOAT;

			case vk::Format::eB8G8R8A8Unorm:
				return TextureFormat::B8G8R8A8_UNORM;

			default:
			{
				TUR_LOG_ERROR("Invalid Texture Format: {}. Default: RGBA (32-bit)", static_cast<int>(format));
				return TextureFormat::RGBA8_UNORM;
			}
		}
	}
}
