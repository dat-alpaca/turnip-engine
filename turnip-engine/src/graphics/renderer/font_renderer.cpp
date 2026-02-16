#include "font_renderer.hpp"
#include "data-structures/range.hpp"
#include "defines.hpp"

#include "graphics/camera.hpp"
#include "graphics/objects/buffer.hpp"
#include "graphics/objects/pipeline.hpp"
#include "graphics/renderer/font_renderer.hpp"

namespace tur
{
	void FontRenderer::initialize(NON_OWNING RenderInterface* rhi)
	{
		commandBuffer = rhi->create_command_buffer();
		commandBuffer.initialize_secondary();

		rRHI = rhi;
		initialize_resources();
	}
	void FontRenderer::set_camera(NON_OWNING Camera* camera)
	{
		rCamera = camera;
	}

	void FontRenderer::render()
	{
		auto& rhi = *rRHI;
		auto& resources = rhi.get_resource_handler();
		
		if(!rCamera)
			return;

		if(mCharacterEntries.empty())
			return;

		// Character data:
		resources.update_buffer(ssbo, mCharacterEntries.data(), Range{ .size = mCharacterEntries.size() * sizeof(CharacterEntry) });
		
		// World data:
		{
			WorldUBO ubo;
			ubo.model = mModel;
			ubo.view = rCamera->view();
			ubo.projection = rCamera->projection();
			
			resources.update_buffer(worldUBO, &ubo, Range{ .size=sizeof(WorldUBO) });
		}
		
		resources.write_storage_buffer_to_set(mainSet, ssbo, Range{ .size = mCharacterEntries.size() * sizeof(CharacterEntry) }, 0);
		resources.write_uniform_buffer_to_set(mainSet, worldUBO, Range{ .size=sizeof(WorldUBO) }, 1);
		
		// Tile data:
		if(fontAtlasHandle == invalid_handle)
			resources.write_texture_to_set(mainSet, rRHI->DefaultTextureHandle, 2);
		else
			resources.write_texture_to_set(mainSet, fontAtlasHandle, 2);
		
		commandBuffer.begin(renderTarget);
		{
			commandBuffer.set_viewport(viewport);
			commandBuffer.set_scissor(scissor);
			commandBuffer.bind_pipeline(pipeline);
			commandBuffer.bind_descriptor_set(mainSet);

			commandBuffer.draw(6 * mCharacterEntries.size(), 1, 0, 0);
		}
		commandBuffer.end();
	}

	void FontRenderer::set_model(const glm::mat4& model)
	{
		mModel = model;
	}
	void FontRenderer::set_font_texture(texture_handle textureHandle)
	{
		fontAtlasHandle = textureHandle;
	}
	void FontRenderer::set_character_data(const std::vector<CharacterEntry>& characters)
	{
		mCharacterEntries = characters;
	}

	void FontRenderer::initialize_resources()
	{
		initialize_buffers();
		initialize_descriptors();
		initialize_pipeline();
	}
	void FontRenderer::initialize_buffers()
	{
		auto& resources = rRHI->get_resource_handler();

		{
			BufferDescriptor descriptor = {
				.type = BufferType::STORAGE_BUFFER | BufferType::TRANSFER_DST,
				.usage = BufferUsage::PERSISTENT | BufferUsage::COHERENT 
			};
			ssbo = resources.create_empty_buffer(descriptor, sizeof(CharacterEntry) * MaxCharacterAmountSSBO);
		}

		{
			BufferDescriptor descriptor = {
				.type = BufferType::UNIFORM_BUFFER | BufferType::TRANSFER_DST,
				.usage = BufferUsage::COHERENT | BufferUsage::PERSISTENT
			};
			worldUBO = resources.create_empty_buffer(descriptor, sizeof(WorldUBO));
		}
	}
	void FontRenderer::initialize_descriptors()
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
	void FontRenderer::initialize_pipeline()
	{
		auto& resources = rRHI->get_resource_handler();

		// Shaders:
		shader_handle vertexShader =
			resources.create_shader({"res/shaders/font/font_vert.spv", ShaderType::VERTEX});
		shader_handle fragmentShader =
			resources.create_shader({"res/shaders/font/font_frag.spv", ShaderType::FRAGMENT});
	
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
				.cullMode = CullMode::NONE,
			},
			.depthDescriptor =
			{
				.depthTestEnable = true,
				.depthWriteEnable = true,
				.compareOp = DepthCompareOp::LESS_OR_EQUAL
			},
			.blendDescriptor = {
				.enable = true
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