#pragma once
#include "defines.hpp"
#include "graphics/objects/descriptor.hpp"
#include "graphics/objects/texture.hpp"
#include "graphics/renderer/renderer.hpp"
#include "graphics/camera.hpp"
#include "rhi/rhi.hpp"
#include "utils/transform.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace tur
{
	class CubemapRenderer : public Renderer
	{
	public:
		using quad_handle = handle_type;

		struct Vertex
		{
			glm::vec3 position;
		};

		struct UBO
		{
			glm::mat4 view;
			glm::mat4 projection;
		};

		struct Cubemap
		{
			descriptor_set_handle setHandle = invalid_handle;
			texture_handle textureHandle = invalid_handle;
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
	
	public:
		void set_cubemap_details(texture_handle handle)
		{
			currentTextureHandle = handle;
		}

	public:
		inline bool is_valid() const { return currentTextureHandle != invalid_handle && rCamera; }

	private:
		NON_OWNING RenderInterface* rRHI = nullptr;
		NON_OWNING Camera* rCamera = nullptr;

	public:
		descriptor_set_layout_handle setLayout;
		buffer_handle vbo, ebo, ubo;
		pipeline_handle pipeline;

	private:
		descriptor_set_handle setHandle = invalid_handle;
		texture_handle currentTextureHandle = invalid_handle;
	};
}