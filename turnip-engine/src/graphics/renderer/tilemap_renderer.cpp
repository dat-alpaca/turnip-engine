#include "tilemap_renderer.hpp"
#include "data-structures/range.hpp"
#include "defines.hpp"
#include "graphics/camera.hpp"
#include "graphics/constants.hpp"
#include "graphics/objects/buffer.hpp"
#include "logging/logging.hpp"
#include <iterator>

namespace tur
{
	void TilemapRenderer::initialize(NON_OWNING RenderInterface* rhi)
	{
		commandBuffer = rhi->create_command_buffer();
		commandBuffer.initialize_secondary();

		rRHI = rhi;
		initialize_resources();
	}
	void TilemapRenderer::set_camera(NON_OWNING Camera* camera)
	{
		rCamera = camera;
	}
	void TilemapRenderer::set_clear_color(const ClearColor& color, ClearFlags flags)
	{
		commandBuffer.set_clear_color(color, flags);
	}

	void TilemapRenderer::render()
	{
		auto& rhi = *rRHI;
		auto& resources = rhi.get_resource_handler();
		
		if(!rCamera)
			return;

		if(mTiles.empty())
			return;

		resources.update_buffer(ssbo, mTiles.data(), Range{ .size = mTiles.size() * sizeof(Tile) });
		
		// World data:
		{
			WorldUBO ubo;
			{
				ubo.viewProjection = rCamera->view() * rCamera->projection();
				ubo.accumulatedMilli = mAccumulatedMilliseconds;
			}
			
			resources.update_buffer(worldUBO, &ubo, Range{ .size=sizeof(WorldUBO) });
		}
		
		resources.write_storage_buffer_to_set(mainSet, ssbo, Range{ .size = mTiles.size() * sizeof(Tile) }, 0);
		resources.write_uniform_buffer_to_set(mainSet, worldUBO, Range{ .size=sizeof(WorldUBO) }, 1);
		
		// Tile data:
		if(atlasHandle == invalid_handle)
			resources.write_texture_to_set(mainSet, rRHI->DefaultTextureHandle, 2);
		else
			resources.write_texture_to_set(mainSet, atlasHandle, 2);
		
		commandBuffer.begin(renderTarget);
		{
			commandBuffer.set_viewport(viewport);
			commandBuffer.set_scissor(scissor);
			commandBuffer.bind_pipeline(pipeline);
			commandBuffer.bind_descriptor_set(mainSet);

			commandBuffer.draw(6 * mTiles.size(), 1, 0, 0);
		}
		commandBuffer.end();
	}

	void TilemapRenderer::set_atlas_texture(texture_handle textureHandle)
	{
		atlasHandle = textureHandle;
	}
	void TilemapRenderer::set_tile_data(const std::vector<Tile>& tiles)
	{
		mTiles = tiles;
	}
	void TilemapRenderer::set_accumulated_time(u32 accumulatedMilli)
	{
		mAccumulatedMilliseconds = accumulatedMilli;
	}

	void TilemapRenderer::initialize_resources()
	{
		initialize_buffers();
		initialize_descriptors();
		initialize_pipeline();
	}
	void TilemapRenderer::initialize_buffers()
	{
		auto& resources = rRHI->get_resource_handler();

		{
			BufferDescriptor descriptor = {
				.type = BufferType::STORAGE_BUFFER | BufferType::TRANSFER_DST,
				.usage = BufferUsage::PERSISTENT | BufferUsage::COHERENT 
			};
			ssbo = resources.create_empty_buffer(descriptor, sizeof(Tile) * MaxTileAmountSSBO);
		}

		{
			BufferDescriptor descriptor = {
				.type = BufferType::UNIFORM_BUFFER | BufferType::TRANSFER_DST,
				.usage = BufferUsage::COHERENT | BufferUsage::PERSISTENT
			};
			worldUBO = resources.create_empty_buffer(descriptor, sizeof(WorldUBO));
		}
	}
	void TilemapRenderer::initialize_descriptors()
	{
		auto& resources = rRHI->get_resource_handler();

		/* Descriptor Set */
		auto LayoutEntries = std::vector<DescriptorSetLayoutEntry>
		({
			{
				.binding = 0,
			  	.amount = 1,
			  	.type = DescriptorType::STORAGE_BUFFER,
			  	.stage = PipelineStage::VERTEX_STAGE
			},
			{
				.binding = 1,
			  	.amount = 1,
			  	.type = DescriptorType::UNIFORM_BUFFER,
			  	.stage = PipelineStage::VERTEX_STAGE
			},
			{
				.binding = 2,
			  	.amount = 1,
			  	.type = DescriptorType::COMBINED_IMAGE_SAMPLER,
			  	.stage = PipelineStage::FRAGMENT_STAGE
			}
		});
		setLayout = rRHI->get_resource_handler().create_descriptor_set_layout({.entries = LayoutEntries});
		mainSet = rRHI->get_resource_handler().create_descriptor_set(setLayout);
	}
	void TilemapRenderer::initialize_pipeline()
	{
		auto& resources = rRHI->get_resource_handler();

		// Shaders:
		shader_handle vertexShader =
			resources.create_shader({"res/shaders/tile/tile_vert.spv", ShaderType::VERTEX});
		shader_handle fragmentShader =
			resources.create_shader({"res/shaders/tile/tile_frag.spv", ShaderType::FRAGMENT});
	
		constexpr auto DynamicStates = std::to_array<DynamicState>({
			DynamicState::VIEWPORT,
			DynamicState::SCISSOR
		});
		
		constexpr auto Viewports = std::to_array<Viewport>({Viewport{}});
		constexpr auto Scissors = std::to_array<Extent2D>({Extent2D{}});

		// Pipeline:
		PipelineDescriptor pipelineDescriptor = {
			.vertexInputStage = 
			{
				.vertexBindings = {},
				.attributes = {}
			},
			.inputAssemblyStage = 
			{
				.topology = PrimitiveTopology::TRIANGLES,
				.primitiveRestartEnable = false
			},
			.rasterizerStage = 
			{
				.frontFace = FrontFace::COUNTER_CLOCKWISE, 
				.cullMode = CullMode::FRONT,
			},
			.setLayout = setLayout,
			.viewports = Viewports,
			.scissors = Scissors,
			.dynamicStates = DynamicStates,
			.vertexShader = vertexShader,
			.fragmentShader = fragmentShader,
		};

		pipeline = resources.create_graphics_pipeline(pipelineDescriptor);
	}
}