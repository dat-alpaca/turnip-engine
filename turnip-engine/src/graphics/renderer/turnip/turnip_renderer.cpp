#include "turnip_renderer.hpp"
#include "defines.hpp"

#include "graphics/mesh_vertex.hpp"
#include "graphics/objects/buffer.hpp"
#include "graphics/renderer/turnip/common.hpp"

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

		/* Resources */
		{
			// Draw buffers:
			resources.update_buffer(drawBuffer, mDrawCommands.data(), Range { .size = mDrawCommands.size() * sizeof(DrawCommand) });

			// World data:
			WorldData data = {
				.view = rCamera->view(),
				.projection = rCamera->projection()
			};
			resources.update_buffer(worldSSBO, &data, Range { .size = sizeof(WorldData) });
			resources.write_storage_buffer_to_set
			(
				mDescriptorSetMap[mPlaceholderPipeline], 
				worldSSBO,
				Range{ .size = sizeof(WorldData) }, 
				0
			);

			// Object data:
			resources.update_buffer(objectSSBO, mObjectData.data(), Range { .size = sizeof(ObjectData) * mObjectData.size() });
			resources.write_storage_buffer_to_set
			(
				mDescriptorSetMap[mPlaceholderPipeline], 
				objectSSBO,
				Range{ .size = sObjectCapacity, .offset = 0 }, 
				1
			);
		}
		
		// Graphics:
		commandBuffer.begin(renderTarget);
		commandBuffer.set_viewport(viewport);
		commandBuffer.set_scissor(scissor);

			/* foreach material get pipeline, bind pipeline */
			commandBuffer.bind_pipeline(mPlaceholderPipeline);

			commandBuffer.bind_vertex_buffer(vbo, 0);
			commandBuffer.bind_index_buffer(ebo, BufferIndexType::UNSIGNED_SHORT);

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

	void TurnipRenderer::set_draw_commands(const std::vector<DrawCommand>& commands)
	{
		mDrawCommands = std::move(commands);
	}
	void TurnipRenderer::clear_objects()
	{
		mObjectData.clear();
	}
	void TurnipRenderer::push_object(const ObjectData& object)
	{
		mObjectData.push_back(object);
	}

	OffsetData TurnipRenderer::upload_mesh(const std::vector<byte>& vertices, const std::vector<byte>& indices)
	{
		auto& resources = rRHI->get_resource_handler();
		resources.update_buffer(vbo, vertices.data(), Range{ .size = vertices.size(), .offset = vertexOffset });
		resources.update_buffer(ebo, indices.data(), Range{ .size = indices.size(), .offset = indicesOffset });
		
		OffsetData data;
		{
			data.baseVertex = vertexOffset / sizeof(MeshVertex);
			data.firstIndex = indicesOffset / sizeof(u16);
		}

		vertexOffset += vertices.size();
		indicesOffset += indices.size();

		return data;
	}
	void TurnipRenderer::clear_buffers()
	{
		auto& resources = rRHI->get_resource_handler();

		vertexOffset = 0;
		indicesOffset = 0;

		if(is_valid_handle(vbo))
			resources.destroy_buffer(vbo);

		if(is_valid_handle(ebo))
			resources.destroy_buffer(ebo);

		if(is_valid_handle(worldSSBO))
			resources.destroy_buffer(worldSSBO);

		prepare_buffers();
	}

	void TurnipRenderer::prepare_buffers()
	{
		auto& resources = rRHI->get_resource_handler();

		BufferDescriptor descriptor =
		{
			.type = BufferType::STORAGE_BUFFER | BufferType::TRANSFER_DST,
			.usage = BufferUsage::PERSISTENT | BufferUsage::COHERENT 
		};
		worldSSBO = resources.create_empty_buffer(descriptor, sizeof(WorldData));
		objectSSBO = resources.create_empty_buffer(descriptor, sObjectCapacity);
		
		BufferDescriptor vboDescriptor =
		{
			.type = BufferType::VERTEX_BUFFER,
			.usage = BufferUsage::PERSISTENT | BufferUsage::COHERENT 
		};
		vbo = resources.create_empty_buffer(vboDescriptor, sVertexCapacity);

		BufferDescriptor eboDescriptor =
		{
			.type = BufferType::INDEX_BUFFER,
			.usage = BufferUsage::PERSISTENT | BufferUsage::COHERENT 
		};
		ebo = resources.create_empty_buffer(eboDescriptor, sIndexCapacity);
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
			},
			{
				// Object Data
				.binding = 1,
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
			.depthDescriptor =
			{
				.depthTestEnable = true
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