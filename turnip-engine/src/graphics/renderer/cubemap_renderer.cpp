#include "cubemap_renderer.hpp"
#include "defines.hpp"
#include "graphics/constants.hpp"
#include "graphics/objects/pipeline.hpp"
#include "graphics/renderer/cubemap_renderer.hpp"

namespace tur
{
	void CubemapRenderer::initialize(NON_OWNING RenderInterface* rhi)
	{
		commandBuffer = rhi->create_command_buffer();
		commandBuffer.initialize_secondary();

		rRHI = rhi;
		initialize_resources();
	}
	void CubemapRenderer::set_camera(NON_OWNING Camera* camera)
	{
		rCamera = camera;
	}
	void CubemapRenderer::set_clear_color(const ClearColor& color, ClearFlags flags)
	{
		commandBuffer.set_clear_color(color, flags);
	}

	void CubemapRenderer::render()
	{
		auto& rhi = *rRHI;
		auto& resources = rhi.get_resource_handler();

		if(!rCamera || currentTextureHandle == invalid_handle)
			return;

		UBO uboData;
		{
			glm::mat4 untranslatedView = glm::mat4(glm::mat3(rCamera->view()));

			uboData.projection = rCamera->projection();
			uboData.view = untranslatedView;

			resources.update_buffer(ubo, &uboData, {.size = sizeof(UBO)});
			resources.write_uniform_buffer_to_set(setHandle, ubo, {.size = sizeof(UBO)}, 0);

			resources.write_texture_to_set(setHandle, currentTextureHandle, 1);
		}

		commandBuffer.begin(renderTarget);

			commandBuffer.set_viewport(viewport);
			commandBuffer.set_scissor(scissor);

			commandBuffer.bind_vertex_buffer(vbo, 0);
			commandBuffer.bind_pipeline(pipeline);

			commandBuffer.bind_descriptor_set(setHandle);
			commandBuffer.draw(36, 1, 0, 0);

		commandBuffer.end();
	}

	void CubemapRenderer::initialize_resources()
	{
		initialize_buffers();
		initialize_descriptors();
		initialize_pipeline();
	}
	void CubemapRenderer::initialize_buffers()
	{
		auto& resources = rRHI->get_resource_handler();

		{
			BufferDescriptor descriptor = {
				.type = BufferType::VERTEX_BUFFER,
			};

				std::array<Vertex, 36> vertices = 
				{
                    Vertex{glm::vec3{ -1.0f,  1.0f, -1.0f }},
                    Vertex{glm::vec3{ -1.0f, -1.0f, -1.0f }},
                    Vertex{glm::vec3{  1.0f, -1.0f, -1.0f }},
                    Vertex{glm::vec3{  1.0f, -1.0f, -1.0f }},
                    Vertex{glm::vec3{  1.0f,  1.0f, -1.0f }},
                    Vertex{glm::vec3{ -1.0f,  1.0f, -1.0f }},

                    Vertex{glm::vec3{ -1.0f, -1.0f,  1.0f }},
                    Vertex{glm::vec3{ -1.0f, -1.0f, -1.0f }},
                    Vertex{glm::vec3{ -1.0f,  1.0f, -1.0f }},
                    Vertex{glm::vec3{ -1.0f,  1.0f, -1.0f }},
                    Vertex{glm::vec3{ -1.0f,  1.0f,  1.0f }},
                    Vertex{glm::vec3{ -1.0f, -1.0f,  1.0f }},

                    Vertex{glm::vec3{ 1.0f, -1.0f, -1.0f }},
                    Vertex{glm::vec3{ 1.0f, -1.0f,  1.0f }},
                    Vertex{glm::vec3{ 1.0f,  1.0f,  1.0f }},
                    Vertex{glm::vec3{ 1.0f,  1.0f,  1.0f }},
                    Vertex{glm::vec3{ 1.0f,  1.0f, -1.0f }},
                    Vertex{glm::vec3{ 1.0f, -1.0f, -1.0f }},

                    Vertex{glm::vec3{ -1.0f, -1.0f,  1.0f }},
                    Vertex{glm::vec3{ -1.0f,  1.0f,  1.0f }},
                    Vertex{glm::vec3{  1.0f,  1.0f,  1.0f }},
                    Vertex{glm::vec3{  1.0f,  1.0f,  1.0f }},
                    Vertex{glm::vec3{  1.0f, -1.0f,  1.0f }},
                    Vertex{glm::vec3{ -1.0f, -1.0f,  1.0f }},

                    Vertex{glm::vec3{ -1.0f,  1.0f, -1.0f }},
                    Vertex{glm::vec3{  1.0f,  1.0f, -1.0f }},
                    Vertex{glm::vec3{  1.0f,  1.0f,  1.0f }},
                    Vertex{glm::vec3{  1.0f,  1.0f,  1.0f }},
                    Vertex{glm::vec3{ -1.0f,  1.0f,  1.0f }},
                    Vertex{glm::vec3{ -1.0f,  1.0f, -1.0f }},

                    Vertex{glm::vec3{ -1.0f, -1.0f, -1.0f }},
                    Vertex{glm::vec3{ -1.0f, -1.0f,  1.0f }},
                    Vertex{glm::vec3{  1.0f, -1.0f, -1.0f }},
                    Vertex{glm::vec3{  1.0f, -1.0f, -1.0f }},
                    Vertex{glm::vec3{ -1.0f, -1.0f,  1.0f }},
                    Vertex{glm::vec3{  1.0f, -1.0f,  1.0  }},
				};

			vbo = resources.create_buffer(descriptor, vertices.data(), {.size = sizeof(Vertex) * vertices.size()});
		}

		{
			BufferDescriptor descriptor = {
				.type = BufferType::UNIFORM_BUFFER | BufferType::TRANSFER_DST,
				.usage = BufferUsage::COHERENT | BufferUsage::PERSISTENT};
			ubo = resources.create_empty_buffer(descriptor, sizeof(UBO));
		}
	}
	void CubemapRenderer::initialize_descriptors()
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
		setHandle = resources.create_descriptor_set(setLayout);
	}
	void CubemapRenderer::initialize_pipeline()
	{
		auto& resources = rRHI->get_resource_handler();

		// Shaders:
		shader_handle vertexShader =
			resources.create_shader({"res/shaders/cubemap/cubemap_vert.spv", ShaderType::VERTEX});
		shader_handle fragmentShader =
			resources.create_shader({"res/shaders/cubemap/cubemap_frag.spv", ShaderType::FRAGMENT});

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
			.depthDescriptor = 
			{
				.depthTestEnable = true,
				.depthWriteEnable = true,
				.compareOp = DepthCompareOp::LESS_OR_EQUAL,
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