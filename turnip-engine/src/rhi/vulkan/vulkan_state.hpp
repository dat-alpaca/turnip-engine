#pragma once
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include "objects/texture.hpp"
#include "objects/render_target.hpp"
#include "types/queue_list.hpp"

namespace tur::vulkan
{
	struct VulkanState
	{
		// Instance:
		u32 apiVersion;
		vk::Instance instance;
		vk::DebugUtilsMessengerEXT debugMessenger;
		bool requiresDrawing;
		bool validationEnabled;

		// Surface:
		VkSurfaceKHR surface;

		// Physical Device:
		vk::PhysicalDevice physicalDevice;

		// Logical Device:
		vk::Device logicalDevice;
		QueueList queueList;

		// VMA:
		VmaAllocator vmaAllocator;

		// Swapchain:
		vk::SwapchainKHR swapchain;
		vk::SurfaceFormatKHR swapchainFormat;
		vk::Extent2D swapchainExtent;
		std::vector<Texture> swapchainTextures;

		// Command Pool:
		vk::CommandPool commandPool;

		// Main Render Target:
		render_target_handle mainRenderTargetHandle = invalid_handle;
		RenderTarget mainRenderTarget;
	};
}