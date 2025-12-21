#pragma once
#include "defines.hpp"
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
			alignas(4) u32 accumulatedMilli;
		};

	public:
		void initialize(NON_OWNING RenderInterface* rhi);
		void set_camera(NON_OWNING Camera* camera);
		void set_clear_color(const ClearColor& color, ClearFlags flags);
		void render();

    public:
		void set_atlas_texture(texture_handle textureHandle);
        void set_tile_data(const std::vector<Tile>& tiles);
		void set_accumulated_time(u32 accumulatedMilli);

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
		texture_handle atlasHandle = invalid_handle;

		u32 mAccumulatedMilliseconds = 0;
	
	private:
		static inline constexpr u32 MaxTileAmountSSBO = 1024;	
	};
}