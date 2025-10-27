#pragma once
#include "graphics/camera.hpp"
#include "rhi/rhi.hpp"
#include "utils/transform.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace tur
{
	class ImmQuadRenderer
	{
	public:
		using quad_handle = handle_type;

		struct Vertex
		{
			glm::vec3 position;
			glm::vec2 uvs;
		};

		struct UBO
		{
			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 projection;
		};

		struct Quad
		{
			Transform transform;

			descriptor_set_handle setHandle = invalid_handle;
			texture_handle textureHandle = invalid_handle;
			buffer_handle uboHandle = invalid_handle;
		};

		// TODO: remove and use spirv-tools
		struct ShaderInformation
		{
			std::string vertexGL = "res/shaders/basic/basic_gl.vert";
			std::string fragGL = "res/shaders/basic/basic_gl.frag";

			std::string vertexVK = "res/shaders/basic/basic_vert.spv";
			std::string fragVK = "res/shaders/basic/basic_frag.spv";
#ifdef TUR_GRAPHICS_VK
			bool isVKSetup = true;
#else
			bool isVKSetup = false
#endif
		};

	public:
		void initialize(NON_OWNING RenderInterface* rhi, const ShaderInformation& info);
		void set_camera(NON_OWNING Camera* camera);
		void set_viewport(const Viewport& viewport);
		void set_scissor(const Extent2D& scissor);
		void set_clear_color(const ClearColor& color, ClearFlags flags);
		void render();

	public:
		quad_handle add_quad(const Transform& transform, texture_handle textureHandle);
		Quad& get_quad(quad_handle handle);
		void clear();

	private:
		void initialize_resources();
		void initialize_buffers();
		void initialize_descriptors();
		void initialize_pipeline();

	private:
		NON_OWNING RenderInterface* rRHI = nullptr;
		NON_OWNING Camera* rCamera = nullptr;
		CommandBuffer mCommandBuffer;

	private:
		std::vector<Quad> mQuads;
		Viewport mViewport;
		Extent2D mScissor;

		// TODO: remove
		ShaderInformation mInfo;

	private:
		descriptor_set_layout_handle setLayout;
		queue_handle graphicsQueue, presentQueue;
		buffer_handle vbo, ebo;
		texture_handle defaultTextureHandle;
		pipeline_handle pipeline;
	};
}