#include "turnip_renderer.hpp"

namespace tur
{
	void TurnipRenderer::initialize(NON_OWNING RenderInterface* rhi)
	{
		commandBuffer = rhi->create_command_buffer();
		commandBuffer.initialize_secondary();

		rRHI = rhi;
		initialize_resources();
	}
	void TurnipRenderer::set_camera(NON_OWNING Camera* camera)
	{
		rCamera = camera;
	}

	void TurnipRenderer::render()
	{
		auto& rhi = *rRHI;
		auto& resources = rhi.get_resource_handler();

		if(!rCamera)
			return;

		commandBuffer.begin(renderTarget);
			commandBuffer.set_viewport(viewport);
			commandBuffer.set_scissor(scissor);

			/* For each material in the scene */
			commandBuffer.bind_pipeline(pipeline);

				commandBuffer.bind_descriptor_set(globalSet);
				


		commandBuffer.end();
	}

	void TurnipRenderer::initialize_resources()
	{
		initialize_buffers();
		initialize_descriptors();
		initialize_pipeline();
	}
	void TurnipRenderer::initialize_buffers()
	{
		auto& resources = rRHI->get_resource_handler();

		{
			BufferDescriptor descriptor = 
			{
				.type = BufferType::STORAGE_BUFFER | BufferType::TRANSFER_DST,
				.usage = BufferUsage::PERSISTENT | BufferUsage::COHERENT 
			};

			vertexSSBO = resources.create_empty_buffer(descriptor, sizeof(Vertex) * s_MaxVertexCount);
		}

		{
			BufferDescriptor descriptor = 
			{
				.type = BufferType::STORAGE_BUFFER | BufferType::TRANSFER_DST,
				.usage = BufferUsage::PERSISTENT | BufferUsage::COHERENT 
			};

			objectSSBO = resources.create_empty_buffer(descriptor, sizeof(Vertex) * s_MaxObjectCount);
		}

		{
			BufferDescriptor descriptor = 
			{
				.type = BufferType::STORAGE_BUFFER | BufferType::TRANSFER_DST,
				.usage = BufferUsage::COHERENT | BufferUsage::PERSISTENT
			};

			worldUBO = resources.create_empty_buffer(descriptor, sizeof(World));
		}
	}
	void TurnipRenderer::initialize_descriptors()
	{
		auto& resources = rRHI->get_resource_handler();

		/* Descriptor Set */
		auto LayoutEntries = std::vector<DescriptorSetLayoutEntry>
		({
			{
				.binding = 0,
			  	.amount = 1,
			  	.type = DescriptorType::STORAGE_BUFFER,
			  	.stage = PipelineStage::VERTEX_STAGE
			},
			{
				.binding = 1,
			  	.amount = 1,
			  	.type = DescriptorType::STORAGE_BUFFER,
			  	.stage = PipelineStage::VERTEX_STAGE
			},
			{
				.binding = 2,
			  	.amount = 1,
			  	.type = DescriptorType::STORAGE_BUFFER,
			  	.stage = PipelineStage::VERTEX_STAGE
			}
		});

		setLayout = rRHI->get_resource_handler().create_descriptor_set_layout({.entries = LayoutEntries});
		globalSet = rRHI->get_resource_handler().create_descriptor_set(setLayout);
	}
	void TurnipRenderer::initialize_pipeline()
	{
		auto& resources = rRHI->get_resource_handler();

		// Shaders:
		shader_handle vertexShader =
			resources.create_shader({"res/shaders/turnip/turnip_vert.spv", ShaderType::VERTEX});
		shader_handle fragmentShader =
			resources.create_shader({"res/shaders/turnip/turnip_frag.spv", ShaderType::FRAGMENT});
		
		constexpr auto DynamicStates = std::to_array<DynamicState>({
			DynamicState::VIEWPORT,
			DynamicState::SCISSOR
		});
		
		constexpr auto Viewports = std::to_array<Viewport>({Viewport{}});
		constexpr auto Scissors = std::to_array<Extent2D>({Extent2D{}});

		// Pipeline:
		PipelineDescriptor pipelineDescriptor = 
        {
			.vertexInputStage = 
			{
				.vertexBindings = {},
				.attributes = {}
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