#include "imm_quad_renderer.hpp"
#include "graphics/constants.hpp"

namespace tur
{
	void ImmQuadRenderer::initialize(NON_OWNING RenderInterface* rhi, const ShaderInformation& info)
	{
		rRHI = rhi;
		mInfo = info;
		initialize_resources();
	}
	void ImmQuadRenderer::set_camera(NON_OWNING Camera* camera)
	{
		rCamera = camera;
	}
	void ImmQuadRenderer::set_viewport(const Viewport& viewport)
	{
		mViewport = viewport;
	}
	void ImmQuadRenderer::set_scissor(const Extent2D& scissor)
	{
		mScissor = scissor;
	}
	void ImmQuadRenderer::set_clear_color(const ClearColor& color, ClearFlags flags)
	{
		mCommandBuffer.set_clear_color(color, flags);
	}

	void ImmQuadRenderer::render()
	{
		auto& rhi = *rRHI;
		auto& resources = rhi.get_resource_handler();

		// Update Quads:
		for (auto& quad : mQuads)
		{
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
				resources.write_texture_to_set(quad.setHandle, defaultTextureHandle, 1);
		}

		// Render:
		if (!rhi.begin_frame())
			return;

		mCommandBuffer.reset(rhi.get_current_command_buffer());

		mCommandBuffer.set_viewport(mViewport);
		mCommandBuffer.set_scissor(mScissor);
		{
			mCommandBuffer.begin();

			mCommandBuffer.bind_index_buffer(ebo, BufferIndexType::UNSIGNED_INT);
			mCommandBuffer.bind_vertex_buffer(vbo, 0);
			mCommandBuffer.bind_pipeline(pipeline);

			for (const auto& quad : mQuads)
			{
				mCommandBuffer.bind_descriptor_set(quad.setHandle);
				mCommandBuffer.draw_indexed(6, 1, 0, 0);
			}

			mCommandBuffer.end();
		}

		rhi.end_frame();
		rhi.submit(graphicsQueue);
		rhi.present(presentQueue);
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
		mCommandBuffer = rRHI->create_command_buffer();

		graphicsQueue = rRHI->get_queue(QueueOperation::GRAPHICS);
		presentQueue = rRHI->get_queue(QueueOperation::PRESENT);

		initialize_buffers();
		initialize_descriptors();
		initialize_pipeline();

		// Default texture:
		TextureDescriptor descriptor;
		{
			descriptor.width = DefaultTexture::Width;
			descriptor.height = DefaultTexture::Height;
			descriptor.format = DefaultTexture::Format;
			descriptor.tiling = TextureTiling::RAW;
		}

		TextureAsset asset;
		{
			asset.metadata.filepath = "internal/default_texture";
			asset.metadata.uuid = UUID();

			asset.width = DefaultTexture::Width;
			asset.height = DefaultTexture::Height;
			asset.channels = DefaultTexture::Channels;

			u32 size = asset.width * asset.height * asset.channels;
			asset.data = std::vector<byte>(DefaultTexture::Data, DefaultTexture::Data + size);
		}

		defaultTextureHandle = rRHI->get_resource_handler().create_texture(descriptor, asset);
	}
	void ImmQuadRenderer::initialize_buffers()
	{
		auto& resources = rRHI->get_resource_handler();

		{
			BufferDescriptor descriptor = {
				.type = BufferType::VERTEX_BUFFER,
			};

			// clang-format off
				std::array<Vertex, 4> vertices = 
				{
					Vertex{glm::vec3{ -0.5f, -0.5f, 0.0f },	glm::vec2{ 0.0f, 0.0f }},
					Vertex{glm::vec3{  0.5f, -0.5f, 0.0f }, glm::vec2{ 1.0f, 0.0f }},
					Vertex{glm::vec3{  0.5f,  0.5f, 0.0f }, glm::vec2{ 1.0f, 1.0f }},
					Vertex{glm::vec3{ -0.5f,  0.5f, 0.0f }, glm::vec2{ 0.0f, 1.0f }},
				};
			// clang-format on

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
		// clang-format off
		constexpr auto LayoutEntries = std::to_array<const DescriptorSetLayoutEntry>
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
		// clang-format on
		setLayout = rRHI->get_resource_handler().create_descriptor_set_layout({.entries = LayoutEntries});
	}
	void ImmQuadRenderer::initialize_pipeline()
	{
		// clang-format off
		auto& resources = rRHI->get_resource_handler();

		// TODO: remove
		// Shaders:

		shader_handle vertexShader;
		shader_handle fragmentShader;
		if(mInfo.isVKSetup)
		{
			vertexShader =
				resources.create_shader({mInfo.vertexVK, ShaderType::VERTEX});
			fragmentShader =
				resources.create_shader({mInfo.fragVK, ShaderType::FRAGMENT});
		}
		else
		{
			vertexShader =
				resources.create_shader({mInfo.vertexGL, ShaderType::VERTEX});
			fragmentShader =
				resources.create_shader({mInfo.fragGL, ShaderType::FRAGMENT});
		}
		

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
		// clang-format on
	}
}