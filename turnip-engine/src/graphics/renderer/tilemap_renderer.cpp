#include "tilemap_renderer.hpp"
#include "graphics/camera.hpp"
#include "graphics/constants.hpp"
#include "graphics/objects/buffer.hpp"

namespace tur
{
	void TilemapRenderer::initialize(NON_OWNING RenderInterface* rhi, u64 maxTilemapSize)
	{
		mMaxTilemapSize = maxTilemapSize;
		rRHI = rhi;
		initialize_resources();
	}
	void TilemapRenderer::set_camera(NON_OWNING Camera* camera)
	{
		rCamera = camera;
	}
	void TilemapRenderer::set_viewport(const Viewport& viewport)
	{
		mViewport = viewport;
	}
	void TilemapRenderer::set_scissor(const Extent2D& scissor)
	{
		mScissor = scissor;
	}
	void TilemapRenderer::set_clear_color(const ClearColor& color, ClearFlags flags)
	{
		mCommandBuffer.set_clear_color(color, flags);
	}

	void TilemapRenderer::render()
	{
		auto& rhi = *rRHI;
		auto& resources = rhi.get_resource_handler();

		// Render:
		if (!rhi.begin_frame())
			return;

		mCommandBuffer.reset(rhi.get_current_command_buffer());

		mCommandBuffer.set_viewport(mViewport);
		mCommandBuffer.set_scissor(mScissor);
		{
			mCommandBuffer.begin();

			mCommandBuffer.draw(mMaxTilemapSize * 6, 1, 0, 0);
			mCommandBuffer.end();
		}

		rhi.end_frame();
		rhi.submit(graphicsQueue);
		rhi.present(presentQueue);
	}

	void TilemapRenderer::initialize_resources()
	{
		mCommandBuffer = rRHI->create_command_buffer();

		graphicsQueue = rRHI->get_queue(QueueOperation::GRAPHICS);
		presentQueue = rRHI->get_queue(QueueOperation::PRESENT);

		initialize_buffers();
		initialize_descriptors();
		initialize_pipeline();
	}
	void TilemapRenderer::initialize_buffers()
	{
		auto& resources = rRHI->get_resource_handler();

		{
			BufferDescriptor descriptor = {
				.type = BufferType::STORAGE_BUFFER,
				.usage = BufferUsage::PERSISTENT | BufferUsage::WRITE | BufferUsage::COHERENT 
			};
			ssbo = resources.create_buffer(descriptor, nullptr, {
				.size = sizeof(Tile) * mMaxTilemapSize
			});
		}

		{
			BufferDescriptor descriptor = {
				.type = BufferType::UNIFORM_BUFFER,
			};
			cameraUBO = resources.create_buffer(descriptor, nullptr, {.size = sizeof(CameraUBO)});
		}

		
	}
	void TilemapRenderer::initialize_descriptors()
	{
		auto& resources = rRHI->get_resource_handler();

		/* Descriptor Set */
		constexpr auto LayoutEntries = std::to_array<const DescriptorSetLayoutEntry>
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
			  	.type = DescriptorType::UNIFORM_BUFFER,
			  	.stage = PipelineStage::VERTEX_STAGE
			},
			{
				.binding = 2,
			  	.amount = 1,
			  	.type = DescriptorType::COMBINED_IMAGE_SAMPLER,
			  	.stage = PipelineStage::FRAGMENT_STAGE
			}
		});
		setLayout = rRHI->get_resource_handler().create_descriptor_set_layout({.entries = LayoutEntries});
	}
	void TilemapRenderer::initialize_pipeline()
	{
		auto& resources = rRHI->get_resource_handler();

		// Shaders:
		shader_handle vertexShader =
			resources.create_shader({"res/shaders/tile/tile_vert.spv", ShaderType::VERTEX});
		shader_handle fragmentShader =
			resources.create_shader({"res/shaders/tile/tile_frag.spv", ShaderType::FRAGMENT});
		
		auto VertexBindings = std::vector<BindingDescriptor>
		({
			{
				.binding = 0,
				.stride = sizeof(u32), 
				.inputRate = InputRate::VERTEX
			},
		});

		auto Attributes = std::vector<Attribute>
		({
			{
				.binding = 0,
				.location = 0,
				.offset = 0,
				.format = AttributeFormat::R32_UINT
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