#include "turnip_renderer.hpp"
#include "defines.hpp"

#include "graphics/mesh_vertex.hpp"
#include "graphics/objects/buffer.hpp"
#include "graphics/objects/pipeline.hpp"
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
	}
	void TurnipRenderer::set_camera(NON_OWNING Camera* camera)
	{
		rCamera = camera;
	}

	void TurnipRenderer::render()
	{
		auto& rhi = *rRHI;
		auto& resources = rhi.get_resource_handler();

		mHasCommands = false;
		if(!rCamera)
			return;

		/* Resources */
		mWorldData.view = rCamera->view();
		mWorldData.projection = rCamera->projection();
		resources.update_buffer(worldSSBO, &mWorldData, Range { .size = sizeof(WorldData) });

		/* Graphics */
		commandBuffer.begin(renderTarget);
		commandBuffer.set_viewport(viewport);
		commandBuffer.set_scissor(scissor);

		for(const auto& [material, instruction] : mDrawInstructionMap)
		{
			if(is_empty(material))
				continue;

			/* Resources */
			// Draw buffer:
			resources.update_buffer(
				drawBuffer, 
				instruction.drawCommands.data(), Range { .size = instruction.drawCommands.size() * sizeof(DrawCommand) }
			);

			// World:
			resources.write_storage_buffer_to_set
			(
				instruction.set, 
				worldSSBO,
				Range{ .size = sizeof(WorldData) }, 
				0
			);

			// Objects:
			resources.update_buffer
			(
				objectSSBO, 
				instruction.objectData.data(), 
				Range { .size = sizeof(ObjectData) * instruction.objectData.size() }
			);
			resources.write_storage_buffer_to_set
			(
				instruction.set, 
				objectSSBO,
				Range{ .size = sObjectCapacity, .offset = 0 }, 
				1
			);

			// Graphics:
			commandBuffer.bind_pipeline(material);

			commandBuffer.bind_vertex_buffer(vbo, 0);
			commandBuffer.bind_index_buffer(ebo, BufferIndexType::UNSIGNED_SHORT);

			commandBuffer.bind_descriptor_set(instruction.set);
			commandBuffer.draw_indexed_indirect(drawBuffer, 0, instruction.drawCommands.size(), 0);
		}

		commandBuffer.end();
		mHasCommands = true;
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

	void TurnipRenderer::set_draw_commands(material_handle material, const std::vector<DrawCommand>& commands)
	{
		mDrawInstructionMap[material].drawCommands = std::move(commands);
	}
	void TurnipRenderer::clear_objects(material_handle material)
	{
		mDrawInstructionMap[material].objectData.clear();
	}
	void TurnipRenderer::clear_all_objects()
	{
		for(auto& [_, instruction] : mDrawInstructionMap)
			instruction.objectData.clear();
	}
	void TurnipRenderer::push_object(material_handle material, const ObjectData& object)
	{
		mDrawInstructionMap[material].objectData.push_back(object);
	}

	OffsetData TurnipRenderer::upload_mesh(material_handle material, const std::vector<byte>& vertices, const std::vector<byte>& indices)
	{
		auto& drawInstruction = mDrawInstructionMap.at(material);

		auto& resources = rRHI->get_resource_handler();
		resources.update_buffer(vbo, vertices.data(), Range{ .size = vertices.size(), .offset = drawInstruction.vertexOffset });
		resources.update_buffer(ebo, indices.data(), Range{ .size = indices.size(), .offset = drawInstruction.indicesOffset });
		
		OffsetData data;
		{
			data.baseVertex = drawInstruction.vertexOffset / sizeof(MeshVertex);
			data.firstIndex = drawInstruction.indicesOffset / sizeof(u16);
		}

		drawInstruction.vertexOffset += vertices.size();
		drawInstruction.indicesOffset += indices.size();

		return data;
	}
	void TurnipRenderer::clear_all_buffers()
	{
		for(auto& [_, instruction] : mDrawInstructionMap)
		{
			instruction.vertexOffset = 0;
			instruction.indicesOffset = 0;
		}

		auto& resources = rRHI->get_resource_handler();
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
}