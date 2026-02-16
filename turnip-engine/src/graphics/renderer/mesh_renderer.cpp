#include "defines.hpp"
#include "mesh_renderer.hpp"
#include "glm/matrix.hpp"
#include "graphics/objects/buffer.hpp"
#include "graphics/objects/descriptor.hpp"
#include "graphics/objects/material.hpp"

namespace tur
{
	void MeshRenderer::initialize(NON_OWNING RenderInterface* rhi)
	{
		commandBuffer = rhi->create_command_buffer();
		commandBuffer.initialize_secondary();

		rRHI = rhi;
		initialize_resources();
	}
	void MeshRenderer::set_camera(NON_OWNING Camera* camera)
	{
		rCamera = camera;
	}

	void MeshRenderer::render()
	{
		auto& rhi = *rRHI;
		auto& resources = rhi.get_resource_handler();

		if(!rCamera)
			return;

		glm::mat4 vp = rCamera->projection() * rCamera->view();
		for(auto& mesh : mMeshCache)
		{
			UBO ubo;
			{
				ubo.model = mesh.transform;
				ubo.view = rCamera->view();
				ubo.projection = rCamera->projection();
				ubo.cameraPosition = rCamera->position;
				ubo.inverseTranspose = glm::inverse(glm::transpose(ubo.model));
			}

			if(mesh.ubo == invalid_handle)
			{
				BufferDescriptor descriptor;
				descriptor.type = BufferType::UNIFORM_BUFFER | BufferType::TRANSFER_DST;
				descriptor.usage = BufferUsage::COHERENT | BufferUsage::PERSISTENT;

				mesh.ubo = resources.create_empty_buffer(descriptor, sizeof(UBO));
			}

			if(mesh.setHandle == invalid_handle)
				mesh.setHandle = resources.create_descriptor_set(setLayout);

			if(mesh.material.is_valid())
				write_material(mesh.setHandle, mesh.material);

			resources.update_buffer(mesh.ubo, &ubo, {.size = sizeof(UBO)});
			resources.write_uniform_buffer_to_set(mesh.setHandle, mesh.ubo, {.size = sizeof(UBO)}, 0);
		}

		commandBuffer.begin(renderTarget);

			commandBuffer.set_viewport(viewport);
			commandBuffer.set_scissor(scissor);

            for(const auto& data : mMeshCache.available())
            {
				if(data.vbo == invalid_handle || data.ebo == invalid_handle)	
					continue;

                commandBuffer.bind_pipeline(pipeline);
                commandBuffer.bind_index_buffer(data.ebo, data.indexType);
                commandBuffer.bind_vertex_buffer(data.vbo, 0);

                commandBuffer.bind_descriptor_set(data.setHandle);
                commandBuffer.draw_indexed(data.indexCount, 1, 0, 0);
            }
			
		commandBuffer.end();
	}

	void MeshRenderer::initialize_resources()
	{
		initialize_descriptors();
		initialize_pipeline();
	}
	void MeshRenderer::initialize_descriptors()
	{
		auto& resources = rRHI->get_resource_handler();

		/* Descriptor Set */
		auto LayoutEntries = std::vector<DescriptorSetLayoutEntry>
		({
			{
				.binding = 0,
			  	.amount = 1,
			  	.type = DescriptorType::UNIFORM_BUFFER,
			  	.stage = PipelineStage::VERTEX_STAGE
			},
			{
				.binding = 1,
			  	.amount = 1,
			  	.type = DescriptorType::COMBINED_IMAGE_SAMPLER,
			  	.stage = PipelineStage::FRAGMENT_STAGE
			},
			{
				.binding = 2,
			  	.amount = 1,
			  	.type = DescriptorType::COMBINED_IMAGE_SAMPLER,
			  	.stage = PipelineStage::FRAGMENT_STAGE
			},
			{
				.binding = 3,
			  	.amount = 1,
			  	.type = DescriptorType::COMBINED_IMAGE_SAMPLER,
			  	.stage = PipelineStage::FRAGMENT_STAGE
			},
			{
				.binding = 4,
			  	.amount = 1,
			  	.type = DescriptorType::COMBINED_IMAGE_SAMPLER,
			  	.stage = PipelineStage::FRAGMENT_STAGE
			}
		});
		setLayout = rRHI->get_resource_handler().create_descriptor_set_layout({.entries = LayoutEntries});
	}
	void MeshRenderer::initialize_pipeline()
	{
		auto& resources = rRHI->get_resource_handler();

		// Shaders:
		shader_handle vertexShader =
			resources.create_shader({"res/shaders/mesh/mesh_vert.spv", ShaderType::VERTEX});
		shader_handle fragmentShader =
			resources.create_shader({"res/shaders/mesh/mesh_frag.spv", ShaderType::FRAGMENT});
		
		auto VertexBindings = std::vector<BindingDescriptor>
		({
			{
				.binding = 0,
				.stride = sizeof(Vertex),
				.inputRate = InputRate::VERTEX
			}
		});

		auto Attributes = std::vector<Attribute>
		({
			{
				.binding = 0,
				.location = 0,
				.offset = offsetof(Vertex, position),
				.format = AttributeFormat::R32G32B32_SFLOAT
			},
			{
				.binding = 0,
				.location = 1,
				.offset = offsetof(Vertex, normal),
				.format = AttributeFormat::R32G32B32_SFLOAT
			},
			{
				.binding = 0,
				.location = 2,
				.offset = offsetof(Vertex, uv),
				.format = AttributeFormat::R32G32_SFLOAT
			}
		});

		constexpr auto DynamicStates = std::to_array<DynamicState>({
			DynamicState::VIEWPORT,
			DynamicState::SCISSOR
		});
		
		constexpr auto Viewports = std::to_array<Viewport>({Viewport{}});
		constexpr auto Scissors = std::to_array<Extent2D>({Extent2D{}});

		// Pipeline:
		PipelineDescriptor pipelineDescriptor = {
			.vertexInputStage = 
			{
				.vertexBindings = VertexBindings,
				.attributes = Attributes
			},
			.inputAssemblyStage = 
			{
				.topology = PrimitiveTopology::TRIANGLES,
				.primitiveRestartEnable = false
			},
			.rasterizerStage = 
			{
				.frontFace = FrontFace::COUNTER_CLOCKWISE, 
				.cullMode = CullMode::FRONT,
			},
			.setLayout = setLayout,
			.viewports = Viewports,
			.scissors = Scissors,
			.dynamicStates = DynamicStates,
			.vertexShader = vertexShader,
			.fragmentShader = fragmentShader,
		};

		pipeline = resources.create_graphics_pipeline(pipelineDescriptor);
	}

	void MeshRenderer::write_material(descriptor_set_handle setHandle, const MetallicRoughnessMaterial& material)
	{
		auto& resources = rRHI->get_resource_handler();

		// TODO: use a blank texture rather than the default one.

		// Albedo:
		if(material.albedo == invalid_handle)
			resources.write_texture_to_set(setHandle, rRHI->DefaultTextureHandle, 1);
		else
			resources.write_texture_to_set(setHandle, material.albedo, 1);

		// Normal:
		if(material.normal == invalid_handle)
			resources.write_texture_to_set(setHandle, rRHI->DefaultTextureHandle, 2);
		else
			resources.write_texture_to_set(setHandle, material.normal, 2);

		// Metallic:
		if(material.metallic == invalid_handle)
			resources.write_texture_to_set(setHandle, rRHI->DefaultTextureHandle, 3);
		else
			resources.write_texture_to_set(setHandle, material.metallic, 3);

		// Roughness:
		if(material.roughness == invalid_handle)
			resources.write_texture_to_set(setHandle, rRHI->DefaultTextureHandle, 4);
		else
			resources.write_texture_to_set(setHandle, material.roughness, 4);
	}
}