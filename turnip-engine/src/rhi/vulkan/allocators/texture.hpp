#pragma once
#include <optional>

#include "vulkan/vulkan.hpp"
#include "graphics/objects/texture.hpp"
#include "rhi/vulkan/objects/texture.hpp"

namespace tur::vulkan
{
	std::optional<Texture> allocate_texture(vk::Device device, VmaAllocator allocator, const TextureDescriptor& descriptor);
}