#pragma once
#include "graphics/objects/descriptor.hpp"
#include "graphics/renderer/renderer.hpp"
#include "graphics/camera.hpp"
#include "rhi/rhi.hpp"
#include <limits>

namespace tur
{
	class TurnipRenderer : public Renderer
	{
    public:
        struct Vertex
        {
            glm::vec3 a_position;
            float a_uvx;
            
            glm::vec3 a_normal;
            float a_uvy;
        };

        struct Mesh
        {
            alignas(16) glm::mat4 model;
            alignas(16) glm::mat4 inverseTranspose;
            alignas(16) uint textureID;
        };

        struct World
        {
            alignas(16) glm::mat4 viewProjection;
        };

	public:
		void initialize(NON_OWNING RenderInterface* rhi);
		void set_camera(NON_OWNING Camera* camera);
		void render();

	private:
		void initialize_resources();
		void initialize_buffers();
		void initialize_descriptors();
		void initialize_pipeline();

	private:
		NON_OWNING RenderInterface* rRHI = nullptr;
		NON_OWNING Camera* rCamera = nullptr;

	public:
		descriptor_set_layout_handle setLayout;
        descriptor_set_handle globalSet;
		buffer_handle worldUBO, vertexSSBO, objectSSBO;
        buffer_handle drawCommands;
		pipeline_handle pipeline;

    private:
        static inline constexpr u32 s_MaxVertexCount = 1 << 20;
        static inline constexpr u32 s_MaxObjectCount = 1 << 10;

    private:
        struct IndirectCommand
        {
            u32 vertexCount;
            u32 instanceCount;
            u32 firstVertex;
            u32 firstInstance;
        };

        std::vector<IndirectCommand> mIndirectCommands;
	};
}