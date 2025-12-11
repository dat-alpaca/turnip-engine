#pragma once
#include "graphics/camera.hpp"
#include "rhi/rhi.hpp"
#include "utils/transform.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace tur
{
	class TilemapRenderer
	{
	public:
		struct Tile
		{
			glm::vec2 pos;
			u32 layer;
		};

		struct CameraUBO
		{
			glm::mat4 view;
			glm::mat4 projection;
		};

	public:
		void initialize(NON_OWNING RenderInterface* rhi, u64 maxTilemapSize);
		void set_camera(NON_OWNING Camera* camera);
		void set_viewport(const Viewport& viewport);
		void set_scissor(const Extent2D& scissor);
		void set_clear_color(const ClearColor& color, ClearFlags flags);
		void render();

    public:
        void update_tile_data(const glm::vec2& worldPosition, const Tile& tile);

	private:
		void initialize_resources();
		void initialize_buffers();
		void initialize_descriptors();
		void initialize_pipeline();

	private:
		NON_OWNING RenderInterface* rRHI = nullptr;
		NON_OWNING Camera* rCamera = nullptr;
		CommandBuffer mCommandBuffer;

	private:
		Viewport mViewport;
		Extent2D mScissor;

	private:
		descriptor_set_layout_handle setLayout;
		queue_handle graphicsQueue, presentQueue;

		pipeline_handle pipeline;
		buffer_handle cameraUBO, ssbo;

		u64 mMaxTilemapSize = 0;		
	};
}