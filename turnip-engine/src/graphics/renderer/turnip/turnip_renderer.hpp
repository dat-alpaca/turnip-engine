#pragma once
#include "graphics/objects/descriptor.hpp"
#include "graphics/objects/pipeline.hpp"
#include "graphics/renderer/turnip/common.hpp"
#include "graphics/renderer/renderer.hpp"
#include "graphics/camera.hpp"

#include "defines.hpp"
#include "rhi/rhi.hpp"

namespace tur
{
	struct OffsetData
	{
		u32 baseVertex = invalid_handle;
		u32 firstIndex = invalid_handle;

		handle_type vertexSlotHandle = invalid_handle;
		handle_type instanceSlotHandle = invalid_handle;
	};

	class TurnipRenderer : public Renderer
	{
	public:
		void initialize(NON_OWNING RenderInterface* rhi);
		void set_camera(NON_OWNING Camera* camera);
		
    public:
        void render();
        void dispatch();

	public:
		inline void set_draw_commands(const std::vector<DrawCommand>& commands)
		{
			mDrawCommands = std::move(commands);
		}

		inline OffsetData upload_mesh(const std::vector<byte>& vertices, const std::vector<byte>& indices)
		{
			auto& resources = rRHI->get_resource_handler();
			resources.update_buffer(verticesSSBO, vertices.data(), Range{ .size = vertices.size(), .offset = vertexOffset });
			resources.update_buffer(indicesSSBO, indices.data(), Range{ .size = indices.size(), .offset = indicesOffset });
		
			OffsetData data;
			{
				data.baseVertex = vertexOffset;
				data.firstIndex = indicesOffset;
			}

			vertexOffset += vertices.size();
			indicesOffset += indices.size();

			return data;
		}
		inline void clear_buffers()
		{
			auto& resources = rRHI->get_resource_handler();

			vertexOffset = 0;
			indicesOffset = 0;

			if(is_valid_handle(verticesSSBO))
				resources.destroy_buffer(verticesSSBO);

			if(is_valid_handle(indicesSSBO))
				resources.destroy_buffer(indicesSSBO);

			prepare_buffers();
		}

	private:
		void prepare_buffers();
		void prepare_placeholder_pipeline();

	private:
		NON_OWNING RenderInterface* rRHI = nullptr;
		NON_OWNING Camera* rCamera = nullptr;
		
        CommandBuffer computeCommandBuffer;
    
	private:
		std::vector<DrawCommand> mDrawCommands;

		buffer_handle verticesSSBO = invalid_handle;
		buffer_handle indicesSSBO = invalid_handle;
		buffer_handle drawBuffer  = invalid_handle;

		u32 vertexOffset = 0;
		u32 indicesOffset = 0;
	
	private:
		pipeline_handle mPlaceholderPipeline;
		std::unordered_map<pipeline_handle, descriptor_set_handle> mDescriptorSetMap;
	};
}