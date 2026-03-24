#pragma once
#include "graphics/objects/material.hpp"
#include "graphics/renderer/turnip/common.hpp"
#include "graphics/objects/descriptor.hpp"
#include "graphics/objects/pipeline.hpp"
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
	};

	class TurnipRenderer : public Renderer
	{
	public:
		struct WorldData
		{
			glm::mat4 view;
			glm::mat4 projection;
		};

		struct ObjectData
		{
			alignas(16) glm::mat4 model;
			alignas(16) material_handle material;
		};

	public:
		void initialize(NON_OWNING RenderInterface* rhi);
		void set_camera(NON_OWNING Camera* camera);
		
    public:
        void render();
        void dispatch();

	public:
		void set_draw_commands(material_handle material, const std::vector<DrawCommand>& commands);
		void clear_objects(material_handle material);
		void push_object(material_handle material, const ObjectData& object);
		void clear_all_objects();

		OffsetData upload_mesh(material_handle material, const std::vector<byte>& vertices, const std::vector<byte>& indices);
		void clear_all_buffers();

	private:
		void prepare_buffers();

	public:
		inline bool has_commands() const
		{
			return mHasCommands;
		}

		inline bool is_empty(material_handle material) 
		{ 
			return !mDrawInstructionMap.at(material).vertexOffset || !mDrawInstructionMap.at(material).indicesOffset; 
		}

	private:
		NON_OWNING RenderInterface* rRHI = nullptr;
		NON_OWNING Camera* rCamera = nullptr;
		
        CommandBuffer computeCommandBuffer;
    
	private:
		struct DrawCallInstructions
		{
			std::vector<DrawCommand> drawCommands;
			std::vector<ObjectData> objectData;
			
			pipeline_handle pipeline;
			descriptor_set_layout_handle setLayout;
			descriptor_set_handle set;

			u32 vertexOffset = 0;
			u32 indicesOffset = 0;
		};

		bool mHasCommands = false;

	private:
		buffer_handle vbo    = invalid_handle;
		buffer_handle ebo    = invalid_handle;
		buffer_handle worldSSBO    = invalid_handle;
		buffer_handle objectSSBO   = invalid_handle;

		buffer_handle drawBuffer  = invalid_handle;

	private:
		std::unordered_map<material_handle, DrawCallInstructions> mDrawInstructionMap;
		WorldData mWorldData;

		static inline constexpr u32 sVertexCapacity  = sizeof(float)      * (1 << 24);
		static inline constexpr u32 sIndexCapacity   = sizeof(float)      * (1 << 24);
		static inline constexpr u32 sObjectCapacity  = sizeof(ObjectData) * (1 << 10);
	};
}