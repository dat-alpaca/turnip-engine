#pragma once
#include "graphics/renderer/renderer.hpp"
#include "graphics/camera.hpp"
#include "rhi/rhi.hpp"
#include "utils/transform.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace tur
{
	class ImmQuadRenderer : public Renderer
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

	public:
		void initialize(NON_OWNING RenderInterface* rhi);
		void set_camera(NON_OWNING Camera* camera);
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

	private:
		std::vector<Quad> mQuads;

	public:
		descriptor_set_layout_handle setLayout;
		buffer_handle vbo, ebo;
		texture_handle defaultTextureHandle;
		pipeline_handle pipeline;
	};
}