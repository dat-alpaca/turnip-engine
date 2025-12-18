#pragma once
#include "graphics/renderer/renderer.hpp"
#include "graphics/camera.hpp"
#include "graphics/tile.hpp"

#include "rhi/rhi.hpp"
#include "utils/transform.hpp"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace tur
{
	class TilemapRenderer : public Renderer
	{
	public:
		struct WorldUBO
		{
			alignas(16) glm::mat4 viewProjection;
		};

	public:
		void initialize(NON_OWNING RenderInterface* rhi, u64 maxTilemapSize);
		void set_camera(NON_OWNING Camera* camera);
		void set_clear_color(const ClearColor& color, ClearFlags flags);
		void render();

    public:
        void set_tile_data(const std::vector<Tile>& tiles);

	private:
		void initialize_resources();
		void initialize_buffers();
		void initialize_descriptors();
		void initialize_pipeline();

	private:
		NON_OWNING RenderInterface* rRHI = nullptr;
		NON_OWNING Camera* rCamera = nullptr;
	
	private:
		std::vector<Tile> mTiles;
		
		descriptor_set_layout_handle setLayout;
		descriptor_set_handle mainSet;

		pipeline_handle pipeline;
		buffer_handle worldUBO, ssbo;

		u32 mUploadTileAmount = 16; // TODO: change
		u32 mMaxTilemapSize = 0;
		u32 mTileSize = 32; // TODO : change		
	};
}