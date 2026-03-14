#pragma once
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
			glm::mat4 model;
		};

	public:
		void initialize(NON_OWNING RenderInterface* rhi);
		void set_camera(NON_OWNING Camera* camera);
		
    public:
        void render();
        void dispatch();

	public:
		void set_draw_commands(const std::vector<DrawCommand>& commands);
		void clear_objects();
		void push_object(const ObjectData& object);

		OffsetData upload_mesh(const std::vector<byte>& vertices, const std::vector<byte>& indices);
		void clear_buffers();

	private:
		void prepare_buffers();
		void prepare_placeholder_set();
		void prepare_placeholder_pipeline();

	public:
		bool is_empty() const { return !vertexOffset || !indicesOffset; }

	private:
		NON_OWNING RenderInterface* rRHI = nullptr;
		NON_OWNING Camera* rCamera = nullptr;
		
        CommandBuffer computeCommandBuffer;
    
	private:
		std::vector<DrawCommand> mDrawCommands;
		std::vector<ObjectData> mObjectData;

		buffer_handle vbo    = invalid_handle;
		buffer_handle ebo    = invalid_handle;
		buffer_handle worldSSBO    = invalid_handle;
		buffer_handle objectSSBO   = invalid_handle;

		buffer_handle drawBuffer  = invalid_handle;

		u32 vertexOffset = 0;
		u32 indicesOffset = 0;
	
	private:
		pipeline_handle mPlaceholderPipeline;
		descriptor_set_layout_handle setLayout;
		std::unordered_map<pipeline_handle, descriptor_set_handle> mDescriptorSetMap;

		static inline constexpr u32 sVertexCapacity  = sizeof(float)      * (1 << 24);
		static inline constexpr u32 sIndexCapacity   = sizeof(float)      * (1 << 24);
		static inline constexpr u32 sObjectCapacity  = sizeof(ObjectData) * (1 << 10);
	};
}