#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
#include <optional>

#include "rhi/vulkan/utils/logger.hpp"
#include "texture.hpp"

namespace tur::vulkan
{
	std::optional<Texture> allocate_texture(vk::Device device, VmaAllocator allocator, const TextureDescriptor& descriptor)
    {
        TUR_ASS(device);
		TUR_ASS(allocator);

		TUR_ASS(descriptor.width > 0);
		TUR_ASS(descriptor.height > 0);
		TUR_ASS(descriptor.depth >= 1);
		TUR_ASS(descriptor.mipLevels >= 1);
		TUR_ASS(descriptor.samples >= 1);
		TUR_ASS(descriptor.arrayLayers >= 1);

		Texture texture = {};
		texture.format = get_texture_format(descriptor.format);

		u32 width = descriptor.layerWidth > 0 ? descriptor.layerWidth : descriptor.width;
		u32 height = descriptor.layerHeight > 0 ? descriptor.layerHeight : descriptor.height;

		texture.extent = vk::Extent3D{width, height, descriptor.depth};
		texture.layout = vk::ImageLayout::eUndefined;
		texture.descriptor = descriptor;

		vk::ImageCreateInfo imageCreateInfo = {};
		{
			imageCreateInfo.flags = vk::ImageCreateFlags();
			imageCreateInfo.imageType = get_texture_type(descriptor.type);

			imageCreateInfo.format = texture.format;
			imageCreateInfo.extent = texture.extent;

			imageCreateInfo.mipLevels = descriptor.mipLevels;

			if(descriptor.type == TextureType::CUBE_MAP)
				imageCreateInfo.flags |= vk::ImageCreateFlagBits::eCubeCompatible;

			if(descriptor.type == TextureType::CUBE_MAP)
				imageCreateInfo.arrayLayers = 6;
			else
				imageCreateInfo.arrayLayers = descriptor.arrayLayers;

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

			if (descriptor.tiling == TextureTiling::OPTIMAL)
				imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
			else
				imageCreateInfo.tiling = vk::ImageTiling::eLinear;

			// TODO: descriptor for usage flags.
			vk::ImageUsageFlags usageFlags;
			{
				// usageFlags |= vk::ImageUsageFlagBits::eStorage;
				usageFlags |= vk::ImageUsageFlagBits::eTransferDst;
				usageFlags |= vk::ImageUsageFlagBits::eSampled;
			}
			
			if(descriptor.isAttachment)
			{
				usageFlags |= vk::ImageUsageFlagBits::eTransferSrc;
				usageFlags |= vk::ImageUsageFlagBits::eColorAttachment;
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
			return std::nullopt;
		}

		// Image view:
		vk::ImageViewCreateInfo imageViewCreateInfo = {};
		{
			imageViewCreateInfo.flags = vk::ImageViewCreateFlags();
			imageViewCreateInfo.viewType = get_texture_view_type(descriptor.type);
			imageViewCreateInfo.image = texture.image;
			imageViewCreateInfo.format = texture.format;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = descriptor.mipLevels;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = imageCreateInfo.arrayLayers;
			
			if(descriptor.type == TextureType::CUBE_MAP)
				imageViewCreateInfo.subresourceRange.layerCount = 6;

			// TODO: descriptor aspect flags
			imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		}

		texture.imageView = check_vk_object(device.createImageView(imageViewCreateInfo), "ImageView");

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
			samplerInfo.anisotropyEnable = false; // TODO: pass anisotropy from vulkan device properties
			samplerInfo.maxAnisotropy = 1.0f;

			samplerInfo.compareEnable = false;
			samplerInfo.compareOp = vk::CompareOp::eNever;
			samplerInfo.minLod = 0.0f;
			samplerInfo.maxLod = 0.0f;
			samplerInfo.mipLodBias = 0.0f;

			samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
			samplerInfo.unnormalizedCoordinates = false;
		}

		texture.sampler = check_vk_object(device.createSampler(samplerInfo), "Sampler");

		return texture;
    }
}