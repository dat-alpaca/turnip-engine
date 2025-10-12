#pragma once
#include "objects/pipeline.hpp"
#include "objects/queue.hpp"
#include "objects/texture.hpp"
#include "objects/vulkan_command_buffer.hpp"
#include "objects/vulkan_deletion_queue.hpp"
#include "objects/vulkan_resource_handler.hpp"

#include "graphics/render_interface.hpp"
#include "platform/platform.hpp"
#include "types/frame_data.hpp"
#include "vulkan_state.hpp"

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

		void transition_texture_layout(
			RenderInterfaceVulkan& rhi, Texture& texture, const ImageTransitionDescription& description
		);
	}

	class RenderInterfaceVulkan
	{
		friend void
		utils::submit_immediate_command(RenderInterfaceVulkan& rhi, utils::immediate_cmd_function&& command);

		friend void utils::recreate_swapchain(RenderInterfaceVulkan& rhi);

		friend void utils::transition_texture_layout(
			RenderInterfaceVulkan& rhi, Texture& texture, const utils::ImageTransitionDescription& description
		);

	public:
		void initialize(const ConfigData& configData, Window& window);
		void shutdown();

	public:
		void begin_frame();
		void submit(queue_handle graphicsQueue);
		void present(queue_handle presentQueue);
		void end_frame();

	public:
		CommandBuffer create_command_buffer();
		vk::CommandBuffer get_current_command_buffer();

	public:
		queue_handle get_queue(QueueOperation operation);
		inline ResourceHandler& get_resource_handler() { return mResources; }

	private:
		void create_main_render_target();

	public:
		inline NON_OWNING Window* get_window() { return rWindow; }
		inline FrameDataHolder& get_frame_data() { return mFrameDataHolder; }
		inline const VulkanState& get_state() const { return mState; }
		inline VulkanState& get_state() { return mState; }
		inline DeletionQueue& get_deletion_queue() { return mDeletionQueue; }

	private:
		NON_OWNING Window* rWindow = nullptr;
		FrameDataHolder mFrameDataHolder;
		ResourceHandler mResources;
		DeletionQueue mDeletionQueue;
		VulkanState mState;

	private:
		vk::Fence mImmFence;
		vk::CommandPool mImmCommandPool;
		vk::CommandBuffer mImmCommandBuffer;

	private:
		static inline constexpr u32 RecordingFenceTimeout = 1'000'000'000;
		static inline constexpr u32 ImageAvailableTimeout = 1'000'000'000;
	};
}

namespace tur
{
	using RenderInterface = BaseRenderInterface<vulkan::RenderInterfaceVulkan>;
}