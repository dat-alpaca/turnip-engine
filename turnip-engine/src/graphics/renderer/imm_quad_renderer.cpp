#include "imm_quad_renderer.hpp"
#include "graphics/constants.hpp"

namespace tur
{
	void ImmQuadRenderer::initialize(NON_OWNING RenderInterface* rhi)
	{
		commandBuffer = rhi->create_command_buffer();
		commandBuffer.initialize_secondary();

		rRHI = rhi;
		initialize_resources();
	}
	void ImmQuadRenderer::set_camera(NON_OWNING Camera* camera)
	{
		rCamera = camera;
	}
	void ImmQuadRenderer::set_clear_color(const ClearColor& color, ClearFlags flags)
	{
		commandBuffer.set_clear_color(color, flags);
	}

	void ImmQuadRenderer::render()
	{
		auto& rhi = *rRHI;
		auto& resources = rhi.get_resource_handler();

		if(!rCamera)
			return;

		// Update Quads:
		for (auto& quad : mQuads)
		{
			if(!quad.drawQuad)
				continue;
			
			UBO uboData;
			{
				uboData.model = quad.transform.transform();
				uboData.projection = rCamera->projection();
				uboData.view = rCamera->view();

				resources.update_buffer(quad.uboHandle, &uboData, {.size = sizeof(UBO)});
				resources.write_uniform_buffer_to_set(quad.setHandle, quad.uboHandle, {.size = sizeof(UBO)}, 0);
			}

			if (quad.textureHandle != invalid_handle)
				resources.write_texture_to_set(quad.setHandle, quad.textureHandle, 1);
			else
				resources.write_texture_to_set(quad.setHandle, rRHI->DefaultTextureHandle, 1);
		}

		commandBuffer.begin(renderTarget);

			commandBuffer.set_viewport(viewport);
			commandBuffer.set_scissor(scissor);

			commandBuffer.bind_index_buffer(ebo, BufferIndexType::UNSIGNED_INT);
			commandBuffer.bind_vertex_buffer(vbo, 0);
			commandBuffer.bind_pipeline(pipeline);

			for (const auto& quad : mQuads)
			{
				commandBuffer.bind_descriptor_set(quad.setHandle);
				commandBuffer.draw_indexed(6, 1, 0, 0);
			}

		commandBuffer.end();
	}

	ImmQuadRenderer::quad_handle ImmQuadRenderer::add_quad(const Transform& transform, texture_handle textureHandle)
	{
		auto& resources = rRHI->get_resource_handler();

		descriptor_set_handle quadSet = resources.create_descriptor_set(setLayout);
		buffer_handle quadUBO;
		{
			BufferDescriptor descriptor = {
				.type = BufferType::UNIFORM_BUFFER | BufferType::TRANSFER_DST,
				.usage = BufferUsage::COHERENT | BufferUsage::PERSISTENT};
			quadUBO = resources.create_empty_buffer(descriptor, sizeof(UBO));
		}

		Quad quad;
		{
			quad.transform = transform;
			quad.textureHandle = textureHandle;
			quad.uboHandle = quadUBO;
			quad.setHandle = quadSet;
		}

		mQuads.push_back(quad);
		return mQuads.size() - 1;
	}
	ImmQuadRenderer::Quad& ImmQuadRenderer::get_quad(quad_handle handle)
	{
		TUR_ASSERT(handle >= 0, "Invalid quad handle");
		TUR_ASSERT(handle < mQuads.size(), "Invalid quad handle");

		return mQuads[handle];
	}
	void ImmQuadRenderer::clear()
	{
		mQuads.clear();
	}

	void ImmQuadRenderer::initialize_resources()
	{
		initialize_buffers();
		initialize_descriptors();
		initialize_pipeline();
	}
	void ImmQuadRenderer::initialize_buffers()
	{
		auto& resources = rRHI->get_resource_handler();

		{
			BufferDescriptor descriptor = {
				.type = BufferType::VERTEX_BUFFER,
			};

				std::array<Vertex, 4> vertices = 
				{
					Vertex{glm::vec3{ -0.5f, -0.5f, 0.0f },	glm::vec2{ 0.0f, 0.0f }},
					Vertex{glm::vec3{  0.5f, -0.5f, 0.0f }, glm::vec2{ 1.0f, 0.0f }},
					Vertex{glm::vec3{  0.5f,  0.5f, 0.0f }, glm::vec2{ 1.0f, 1.0f }},
					Vertex{glm::vec3{ -0.5f,  0.5f, 0.0f }, glm::vec2{ 0.0f, 1.0f }},
				};

			vbo = resources.create_buffer(descriptor, vertices.data(), {.size = sizeof(Vertex) * 4});
		}

		{
			BufferDescriptor descriptor = {
				.type = BufferType::INDEX_BUFFER,
			};
			unsigned int indices[6] = {0, 1, 2, 2, 3, 0};
			ebo = resources.create_buffer(descriptor, indices, {.size = sizeof(indices)});
		}
	}
	void ImmQuadRenderer::initialize_descriptors()
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
	void ImmQuadRenderer::initialize_pipeline()
	{
		auto& resources = rRHI->get_resource_handler();

		// Shaders:
		shader_handle vertexShader =
				resources.create_shader({"res/shaders/basic/basic_vert.spv", ShaderType::VERTEX});
			shader_handle fragmentShader =
				resources.create_shader({"res/shaders/basic/basic_frag.spv", ShaderType::FRAGMENT});
		

		auto VertexBindings = std::vector<BindingDescriptor>
		({
			{
				.binding = 0, 
				.stride = sizeof(Vertex), 
				.inputRate = InputRate::VERTEX
			},
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
				.offset = offsetof(Vertex, uvs),
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