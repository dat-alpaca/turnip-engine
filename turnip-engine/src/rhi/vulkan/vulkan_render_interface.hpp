#pragma once
#include "graphics/objects/command_buffer.hpp"
#include "graphics/objects/sync.hpp"

#include "objects/texture.hpp"
#include "objects/vulkan_command_buffer.hpp"
#include "objects/vulkan_deletion_queue.hpp"
#include "objects/vulkan_resource_handler.hpp"

#include "graphics/render_interface.hpp"
#include "platform/platform.hpp"
#include "vulkan_state.hpp"
#include <limits>

namespace tur::vulkan
{
	namespace utils
	{
		struct ImageTransitionDescription
		{
			vk::ImageLayout newLayout;
			vk::PipelineStageFlags2 srcStageMask = vk::PipelineStageFlagBits2::eAllCommands;
			vk::AccessFlags2 srcAccessMask = vk::AccessFlagBits2::eMemoryRead;

			vk::PipelineStageFlags2 dstStageMask = vk::PipelineStageFlagBits2::eAllCommands;
			vk::AccessFlags2 dstAccessMask = vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite;
		};

		using immediate_cmd_function = std::function<void(vk::CommandBuffer recordingBuffer)>;

		void submit_immediate_command(RenderInterfaceVulkan& rhi, immediate_cmd_function&& command);
		void recreate_swapchain(RenderInterfaceVulkan& rhi);
		void transition_texture_layout(RenderInterfaceVulkan& rhi, CommandBufferVulkan commandBuffer, Texture& texture, const ImageTransitionDescription& description);
		void transition_texture_layout_imm(RenderInterfaceVulkan& rhi, Texture& texture, const ImageTransitionDescription& description);
	}

	class RenderInterfaceVulkan
	{
		friend void utils::submit_immediate_command(RenderInterfaceVulkan& rhi, utils::immediate_cmd_function&& command);
		friend void utils::recreate_swapchain(RenderInterfaceVulkan& rhi);
		friend void utils::transition_texture_layout(RenderInterfaceVulkan& rhi, CommandBufferVulkan commandBuffer, Texture& texture, const utils::ImageTransitionDescription& description);
		friend void utils::transition_texture_layout_imm(RenderInterfaceVulkan& rhi, Texture& texture, const utils::ImageTransitionDescription& description);

	public:
		void initialize(const ConfigData& configData, Window& window);
		void shutdown();
		void on_event(Event& event);

	public:
		void begin_frame(fence_handle inFlightFence, u32 fenceTimeout = RecordingFenceTimeout);
		void reset_fence(fence_handle inFlightFence);

		std::optional<image_handle> acquire_swapchain_image(semaphore_handle signalImageReady, u32 imageReadyTimeout = ImageAvailableTimeout);

		void submit(queue_handle graphicsQueue, command_buffer_handle commandBuffer, fence_handle inFlightFence, const std::vector<SubmitSemaphoreData>& waitFor, semaphore_handle signal);
		void present(queue_handle presentQueue, const std::vector<semaphore_handle>& waitFor, image_handle imageHandle);
		void end_frame();

	public:
		CommandBuffer create_command_buffer();

	public:
		queue_handle get_queue(QueueOperation operation);
		inline ResourceHandler& get_resource_handler() { return mResources; }

	public:
		inline HardwareInformation get_hardware_information() const
		{
			return mState.hardwareInformation;
		}

	private:
		void create_main_render_target();

	public:
		inline NON_OWNING Window* get_window() { return rWindow; }
		inline const VulkanState& get_state() const { return mState; }
		inline VulkanState& get_state() { return mState; }
		inline DeletionQueue& get_deletion_queue() { return mDeletionQueue; }

	private:
		NON_OWNING Window* rWindow = nullptr;
		ResourceHandler mResources;
		DeletionQueue mDeletionQueue;
		VulkanState mState;

	private:
		vk::Fence mImmFence;
		vk::CommandPool mImmCommandPool;
		vk::CommandBuffer mImmCommandBuffer;

	private:
		bool mWindowResized = false;

	private:
		static inline constexpr u32 RecordingFenceTimeout = std::numeric_limits<u32>::max();
		static inline constexpr u32 ImageAvailableTimeout = std::numeric_limits<u32>::max();
	};
}

namespace tur
{
	using RenderInterface = BaseRenderInterface<vulkan::RenderInterfaceVulkan>;
}