#pragma once
#include "defines.hpp"
#include "graphics/objects/descriptor.hpp"
#include "graphics/renderer/renderer.hpp"
#include "graphics/objects/buffer.hpp"
#include "graphics/camera.hpp"

#include "data-structures/free_list.hpp"
#include "rhi/rhi.hpp"
#include "utils/uuid/uuid.hpp"

#include <tiny_gltf.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace tur
{
	class ModelRenderer : public Renderer
	{
	public:
		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 uv;
		};

		struct UBO
		{
			glm::mat4 mvp;
		};

		struct InternalModel
		{
			glm::mat4 transform;

			u64 indexCount = 0;
			buffer_handle vbo = invalid_handle;
			buffer_handle ebo = invalid_handle;
			buffer_handle ubo = invalid_handle;

			descriptor_set_handle setHandle = invalid_handle;
			bool isVisible = true;
		};
    
    public:
		void initialize(NON_OWNING RenderInterface* rhi);
		void set_camera(NON_OWNING Camera* camera);
		void set_clear_color(const ClearColor& color, ClearFlags flags);
		void render();

	public:
		handle_type add_model(const InternalModel& data)
		{
			return mModelCache.add(data);
		}
		
		InternalModel& get_model(handle_type uniqueHandle) { return mModelCache.get(uniqueHandle); }

		void remove_model(handle_type uniqueHandle)
		{
			mModelCache.remove(uniqueHandle);
		}
	
	private:
		void initialize_resources();
		void initialize_descriptors();
		void initialize_pipeline();

	private:
		NON_OWNING RenderInterface* rRHI = nullptr;
		NON_OWNING Camera* rCamera = nullptr;

	public:
		descriptor_set_layout_handle setLayout;
		pipeline_handle pipeline;

	private:
		free_list<InternalModel> mModelCache;
	};
}