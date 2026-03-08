#include "turnip_renderer.hpp"
#include "defines.hpp"
#include "graphics/objects/buffer.hpp"
#include "graphics/renderer/turnip/common.hpp"
#include <limits>

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
		drawBuffer = resources.create_empty_buffer(drawDescriptor, std::numeric_limits<u32>::max());
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

		commandBuffer.begin(renderTarget);
			commandBuffer.set_viewport(viewport);
			commandBuffer.set_scissor(scissor);
		
			/* foreach material get pipeline, bind pipeline */
			commandBuffer.bind_pipeline(mPlaceholderPipeline);
			resources.write_storage_buffer_to_set
			(
				mDescriptorSetMap[mPlaceholderPipeline], 
				verticesSSBO,
				Range{ .size = vertexOffset, .offset = 0 }, 
				0
			);

			resources.write_storage_buffer_to_set
			(
				mDescriptorSetMap[mPlaceholderPipeline], 
				verticesSSBO,
				Range{ .size = indicesOffset, .offset = 0 }, 
				1
			);

			commandBuffer.bind_descriptor_set(mDescriptorSetMap[mPlaceholderPipeline]);
			commandBuffer.draw_indirect(drawBuffer, 0, mDrawCommands.size(), sizeof(DrawCommand));
		commandBuffer.end();
	}
	void TurnipRenderer::dispatch()
	{
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
		verticesSSBO = resources.create_empty_buffer(descriptor, std::numeric_limits<u32>::max());
		indicesSSBO = resources.create_empty_buffer(descriptor, std::numeric_limits<u32>::max());
	}
	void TurnipRenderer::prepare_placeholder_pipeline()
	{

	}
}