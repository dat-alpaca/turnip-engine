#pragma once
#include "rhi/vulkan/objects/buffer.hpp"
#include "rhi/vulkan/objects/texture.hpp"

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

namespace tur::vulkan
{
	inline Texture allocate_texture(vk::Device device, VmaAllocator allocator, const TextureDescriptor& descriptor)
	{
		Texture texture = {};
		texture.format = get_texture_format(descriptor.format);
		texture.extent = vk::Extent3D{descriptor.width, descriptor.height, descriptor.depth};
		texture.layout = vk::ImageLayout::eUndefined;
		texture.descriptor = descriptor;

		vk::ImageCreateInfo imageCreateInfo = {};
		{
			imageCreateInfo.flags = vk::ImageCreateFlags();
			imageCreateInfo.imageType = get_texture_type(descriptor.type);

			imageCreateInfo.format = texture.format;
			imageCreateInfo.extent = texture.extent;

			imageCreateInfo.mipLevels = descriptor.mipLevels;
			imageCreateInfo.arrayLayers = 1;

			TUR_ASSERT(descriptor.samples <= 16, "Invalid sample size");
			TUR_ASSERT(descriptor.samples > 0, "Invalid sample size");

			switch (descriptor.samples)
			{
				case 1:
					imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
					break;
				case 2:
					imageCreateInfo.samples = vk::SampleCountFlagBits::e2;
					break;
				case 4:
					imageCreateInfo.samples = vk::SampleCountFlagBits::e4;
					break;
				case 8:
					imageCreateInfo.samples = vk::SampleCountFlagBits::e8;
					break;
				case 16:
					imageCreateInfo.samples = vk::SampleCountFlagBits::e16;
					break;
				case 32:
					imageCreateInfo.samples = vk::SampleCountFlagBits::e32;
					break;
				case 64:
					imageCreateInfo.samples = vk::SampleCountFlagBits::e64;
					break;
				default:
					TUR_LOG_CRITICAL("Texture samples must be a power of 2 and less than 64.");
			}

			// TODO: support linear tiling for CPU data.
			imageCreateInfo.tiling = vk::ImageTiling::eOptimal;

			// TODO: descriptor for usage flags.
			vk::ImageUsageFlags usageFlags;
			{
				usageFlags |= vk::ImageUsageFlagBits::eTransferSrc;
				usageFlags |= vk::ImageUsageFlagBits::eTransferDst;
				usageFlags |= vk::ImageUsageFlagBits::eStorage;
				usageFlags |= vk::ImageUsageFlagBits::eColorAttachment;
				usageFlags |= vk::ImageUsageFlagBits::eSampled;
			}
			imageCreateInfo.usage = usageFlags;
		}

		VmaAllocationCreateInfo imageAllocationInfo = {};
		{
			imageAllocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
			imageAllocationInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		}

		if (vmaCreateImage(
				allocator, reinterpret_cast<const VkImageCreateInfo*>(&imageCreateInfo), &imageAllocationInfo,
				reinterpret_cast<VkImage*>(&texture.image), &texture.allocation, nullptr
			)
			!= VK_SUCCESS)
		{
			TUR_LOG_ERROR("Failed to create image");
			return {};
		}

		// Image view:
		vk::ImageViewCreateInfo imageViewCreateInfo = {};
		{
			imageViewCreateInfo.flags = vk::ImageViewCreateFlags();
			imageViewCreateInfo.viewType = get_texture_view_type(descriptor.type);
			imageViewCreateInfo.image = texture.image;
			imageViewCreateInfo.format = texture.format;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = 1;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = 1;

			// TODO: descriptor aspect flags
			imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		}

		texture.imageView = device.createImageView(imageViewCreateInfo);

		// Sampler:
		vk::SamplerCreateInfo samplerInfo = {};
		{
			samplerInfo.minFilter = get_filter_mode(descriptor.minFilter, true);
			samplerInfo.magFilter = get_filter_mode(descriptor.magFilter, false);
			samplerInfo.mipmapMode = get_mipmap_mode(descriptor.minFilter);

			samplerInfo.addressModeU = get_wrap_mode(descriptor.wrapR);
			samplerInfo.addressModeV = get_wrap_mode(descriptor.wrapS);
			samplerInfo.addressModeW = get_wrap_mode(descriptor.wrapT);

			// TODO: fill in the rest of the sampler options
			samplerInfo.anisotropyEnable;
			samplerInfo.maxAnisotropy;

			samplerInfo.compareEnable;
			samplerInfo.compareOp;
			samplerInfo.minLod = 0.0f;
			samplerInfo.maxLod = 0.0f;
			samplerInfo.mipLodBias = 0.0f;

			samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
			samplerInfo.unnormalizedCoordinates = false;
		}

		texture.sampler = device.createSampler(samplerInfo);

		return texture;
	}
}