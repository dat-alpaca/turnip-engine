#pragma once
#include "defines.hpp"
#include "graphics/objects/descriptor.hpp"
#include "graphics/objects/material.hpp"
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
	class MeshRenderer : public Renderer
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
			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 projection;
			glm::mat3 inverseTranspose;
			glm::vec3 cameraPosition;
		};

		struct InternalMesh
		{
			glm::mat4 transform;

			u64 indexCount = 0;
			buffer_handle vbo = invalid_handle;
			buffer_handle ebo = invalid_handle;
			buffer_handle ubo = invalid_handle;
			BufferIndexType indexType = BufferIndexType::UNSIGNED_BYTE;
	
			descriptor_set_handle setHandle = invalid_handle;
			MetallicRoughnessMaterial material;
			bool isVisible = true;
		};
    
    public:
		void initialize(NON_OWNING RenderInterface* rhi);
		void set_camera(NON_OWNING Camera* camera);
		void render();

	public:
		handle_type add_mesh(const InternalMesh& data)
		{
			return mMeshCache.add(data);
		}
		
		InternalMesh& get_mesh(handle_type uniqueHandle) { return mMeshCache.get(uniqueHandle); }

		void remove_mesh(handle_type uniqueHandle)
		{
			mMeshCache.remove(uniqueHandle);
		}

		inline u32 mesh_count() const { return mMeshCache.size(); }
	
	private:
		void initialize_resources();
		void initialize_descriptors();
		void initialize_pipeline();

	private:
		void write_material(descriptor_set_handle setHandle, const MetallicRoughnessMaterial& material);

	private:
		NON_OWNING RenderInterface* rRHI = nullptr;
		NON_OWNING Camera* rCamera = nullptr;

	public:
		descriptor_set_layout_handle setLayout;
		pipeline_handle pipeline;

	private:
		free_list<InternalMesh> mMeshCache;
	};
}