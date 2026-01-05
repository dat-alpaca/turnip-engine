#include "model_renderer.hpp"
#include "defines.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "graphics/objects/buffer.hpp"
#include <cstddef>

namespace tur
{
	void ModelRenderer::initialize(NON_OWNING RenderInterface* rhi)
	{
		commandBuffer = rhi->create_command_buffer();
		commandBuffer.initialize_secondary();

		rRHI = rhi;
		initialize_resources();
	}
	void ModelRenderer::set_camera(NON_OWNING Camera* camera)
	{
		rCamera = camera;
	}
	void ModelRenderer::set_clear_color(const ClearColor& color, ClearFlags flags)
	{
		commandBuffer.set_clear_color(color, flags);
	}

	void ModelRenderer::render()
	{
		auto& rhi = *rRHI;
		auto& resources = rhi.get_resource_handler();

		if(!rCamera)
			return;

		glm::mat4 vp = rCamera->projection() * rCamera->view();
		for(auto& model : mModelCache)
		{
			UBO ubo;
			glm::mat4 i(1.f);
			i = glm::translate(i, {4, 4, 0});
			ubo.mvp = vp * i;

			if(model.ubo == invalid_handle)
			{
				BufferDescriptor descriptor;
				descriptor.type = BufferType::UNIFORM_BUFFER | BufferType::TRANSFER_DST;
				descriptor.usage = BufferUsage::COHERENT | BufferUsage::PERSISTENT;

				model.ubo = resources.create_empty_buffer(descriptor, sizeof(UBO));
			}

			if(model.setHandle == invalid_handle)
			{
				model.setHandle = resources.create_descriptor_set(setLayout);
			}

			if(model.albedo == invalid_handle)
				resources.write_texture_to_set(model.setHandle, rRHI->DefaultTextureHandle, 1);
			else
				resources.write_texture_to_set(model.setHandle, model.albedo, 1);

			resources.update_buffer(model.ubo, &ubo, {.size = sizeof(UBO)});
			resources.write_uniform_buffer_to_set(model.setHandle, model.ubo, {.size = sizeof(UBO)}, 0);
		}

		commandBuffer.begin(renderTarget);

			commandBuffer.set_viewport(viewport);
			commandBuffer.set_scissor(scissor);

            for(const auto& data : mModelCache.available())
            {
				if(data.vbo == invalid_handle || data.ebo == invalid_handle)	
					continue;

                commandBuffer.bind_pipeline(pipeline);
                commandBuffer.bind_index_buffer(data.ebo, BufferIndexType::UNSIGNED_SHORT);
                commandBuffer.bind_vertex_buffer(data.vbo, 0);

                commandBuffer.bind_descriptor_set(data.setHandle);
                commandBuffer.draw_indexed(data.indexCount, 1, 0, 0);
            }
			
		commandBuffer.end();
	}

	void ModelRenderer::initialize_resources()
	{
		initialize_descriptors();
		initialize_pipeline();
	}
	void ModelRenderer::initialize_descriptors()
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
			}
		});
		setLayout = rRHI->get_resource_handler().create_descriptor_set_layout({.entries = LayoutEntries});
	}
	void ModelRenderer::initialize_pipeline()
	{
		auto& resources = rRHI->get_resource_handler();

		// Shaders:
		shader_handle vertexShader =
			resources.create_shader({"res/shaders/model/model_vert.spv", ShaderType::VERTEX});
		shader_handle fragmentShader =
			resources.create_shader({"res/shaders/model/model_frag.spv", ShaderType::FRAGMENT});
		
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
}