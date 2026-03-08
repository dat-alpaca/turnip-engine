#include "turnip_renderer.hpp"
#include "defines.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "graphics/objects/buffer.hpp"
#include "graphics/renderer/turnip/common.hpp"
#include <limits>
#include <vector>

namespace tur
{
	void TurnipRenderer::initialize(NON_OWNING RenderInterface* rhi)
	{
		rRHI = rhi;

		computeCommandBuffer = rhi->create_command_buffer();
		computeCommandBuffer.initialize_secondary();

		commandBuffer = rhi->create_command_buffer();
		commandBuffer.initialize_secondary();

		/* Draw buffer */
		auto& resources = rRHI->get_resource_handler();
		BufferDescriptor drawDescriptor =
		{
			.type = BufferType::INDIRECT | BufferType::TRANSFER_DST,
			.usage = BufferUsage::PERSISTENT | BufferUsage::COHERENT 
		};
		drawBuffer = resources.create_empty_buffer(drawDescriptor, sizeof(DrawCommand) * 10000);

		prepare_placeholder_set();
		prepare_placeholder_pipeline();
	}
	void TurnipRenderer::set_camera(NON_OWNING Camera* camera)
	{
		rCamera = camera;
	}

	void TurnipRenderer::render()
	{
		auto& rhi = *rRHI;
		auto& resources = rhi.get_resource_handler();

		if(!rCamera)
			return;

		if(is_empty())
			return;

		// Update buffers:
		std::vector<glm::mat4> data;
		glm::mat4 model(1.f);
		model = glm::scale(model, {2.f, 2.f, 2.f});
		data.push_back(model);
		data.push_back(rCamera->view());
		data.push_back(rCamera->projection());
		
		resources.update_buffer(drawBuffer, mDrawCommands.data(), Range { .size = mDrawCommands.size() * sizeof(DrawCommand) });
		
		resources.update_buffer(worldSSBO, data.data(), Range { .size = data.size() * sizeof(glm::mat4) });
		resources.write_storage_buffer_to_set
		(
			mDescriptorSetMap[mPlaceholderPipeline], 
			worldSSBO,
			Range{ .size = sizeof(glm::mat4) *  3, .offset = 0 }, 
			0
		);

		// Graphics:
		commandBuffer.begin(renderTarget);
		commandBuffer.set_viewport(viewport);
		commandBuffer.set_scissor(scissor);

			/* foreach material get pipeline, bind pipeline */
			commandBuffer.bind_pipeline(mPlaceholderPipeline);

			commandBuffer.bind_vertex_buffer(vertexBuffer, 0);
			commandBuffer.bind_index_buffer(indexBuffer, BufferIndexType::UNSIGNED_SHORT);

			commandBuffer.bind_descriptor_set(mDescriptorSetMap[mPlaceholderPipeline]);
			commandBuffer.draw_indexed_indirect(drawBuffer, 0, mDrawCommands.size(), 0);
		commandBuffer.end();
	}
	void TurnipRenderer::dispatch()
	{
		return;
		auto& resources = rRHI->get_resource_handler();
		
		descriptor_set_handle computeDescriptor = 0;
		resources.write_storage_buffer_to_set(computeDescriptor, 0 /* ssbo with bounds */, {}, 0);	

		computeCommandBuffer.begin_compute();

			computeCommandBuffer.bind_descriptor_set(computeDescriptor);
			computeCommandBuffer.bind_pipeline(0);
			computeCommandBuffer.dispatch(0, 0, 0);

		computeCommandBuffer.end();

		commandBuffer.begin();
		commandBuffer.begin_rendering();
		commandBuffer.end();
	}

	void TurnipRenderer::prepare_buffers()
	{
		auto& resources = rRHI->get_resource_handler();

		BufferDescriptor descriptor =
		{
			.type = BufferType::STORAGE_BUFFER | BufferType::TRANSFER_DST,
			.usage = BufferUsage::PERSISTENT | BufferUsage::COHERENT 
		};
		worldSSBO = resources.create_empty_buffer(descriptor, 3 * sizeof(glm::mat4));
		
		BufferDescriptor vertexDesc = 
		{
			.type = BufferType::VERTEX_BUFFER,
			.usage = BufferUsage::PERSISTENT | BufferUsage::COHERENT
		};
		vertexBuffer = resources.create_empty_buffer(vertexDesc, 8 * sizeof(float) * (1 << 14));

		BufferDescriptor indexDesc = 
		{
			.type = BufferType::INDEX_BUFFER,
			.usage = BufferUsage::PERSISTENT | BufferUsage::COHERENT
		};
		indexBuffer = resources.create_empty_buffer(indexDesc, 8 * sizeof(float) * (1 << 14));
	}
	
	void TurnipRenderer::prepare_placeholder_set()
	{
		auto& resources = rRHI->get_resource_handler();

		/* Descriptor Set */
		auto LayoutEntries = std::vector<DescriptorSetLayoutEntry>
		({
			{
				// World Data
				.binding = 0,
			  	.amount = 1,
			  	.type = DescriptorType::STORAGE_BUFFER,
			  	.stage = PipelineStage::VERTEX_STAGE
			}
		});

		setLayout = resources.create_descriptor_set_layout({.entries = LayoutEntries});
	}

	void TurnipRenderer::prepare_placeholder_pipeline()
	{
		auto& resources = rRHI->get_resource_handler();

		// Shaders:
		shader_handle vertexShader =
			resources.create_shader({"res/shaders/turnip/test_vert.spv", ShaderType::VERTEX});
		shader_handle fragmentShader =
			resources.create_shader({"res/shaders/turnip/test_frag.spv", ShaderType::FRAGMENT});
		
		constexpr auto DynamicStates = std::to_array<DynamicState>({
			DynamicState::VIEWPORT,
			DynamicState::SCISSOR
		});
		
		constexpr auto Viewports = std::to_array<Viewport>({Viewport{}});
		constexpr auto Scissors = std::to_array<Extent2D>({Extent2D{}});

		auto VertexBindings = std::vector<BindingDescriptor>
		({
			{
				.binding = 0,
				.stride = sizeof(MeshVertex),
				.inputRate = InputRate::VERTEX
			}
		});

		auto Attributes = std::vector<Attribute>
		({
			{
				.binding = 0,
				.location = 0,
				.offset = offsetof(MeshVertex, position),
				.format = AttributeFormat::R32G32B32_SFLOAT
			},
			{
				.binding = 0,
				.location = 1,
				.offset = offsetof(MeshVertex, uvx),
				.format = AttributeFormat::R32_SFLOAT
			},
			{
				.binding = 0,
				.location = 2,
				.offset = offsetof(MeshVertex, normals),
				.format = AttributeFormat::R32G32B32_SFLOAT
			},
			{
				.binding = 0,
				.location = 3,
				.offset = offsetof(MeshVertex, uvy),
				.format = AttributeFormat::R32_SFLOAT
			},
		});

		// Pipeline:
		PipelineDescriptor pipelineDescriptor = {
			.vertexInputStage =  
			{
				.vertexBindings = VertexBindings,
				.attributes = Attributes
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

		mPlaceholderPipeline = resources.create_graphics_pipeline(pipelineDescriptor);
		
		mDescriptorSetMap[mPlaceholderPipeline] = resources.create_descriptor_set(setLayout);
	}
}