#pragma once
#include "rhi/rhi.hpp"
#include <glm/glm.hpp>

namespace tur
{
	class ImmQuadRenderer
	{
		struct Vertex
		{
			glm::vec3 position;
		};

	public:
		void initialize(NON_OWNING RenderInterface* rhi)
		{
			rRHI = rhi;
			initialize_resources();

			mCommandBuffer.clear(ClearColor{.color = Color(30, 40, 50, 255)}, ClearFlags::COLOR);
		}

		void render()
		{
			auto& rhi = *rRHI;

			rhi.begin_frame();
			mCommandBuffer.reset(rhi.get_current_command_buffer());

			mCommandBuffer.set_viewport(Viewport{0, 0, 640, 480});
			mCommandBuffer.set_scissor(Extent2D{0, 0, 640, 480});
			{
				mCommandBuffer.begin();
				mCommandBuffer.bind_pipeline(pipeline);
				mCommandBuffer.bind_vertex_buffer(vbo, 0);

				// bind_resources()
				// draw()

				mCommandBuffer.end();
			}

			rhi.end_frame();
			rhi.submit(graphicsQueue);
			rhi.present(presentQueue);
		}

	private:
		void initialize_resources()
		{
			mCommandBuffer = rRHI->create_command_buffer();

			graphicsQueue = rRHI->get_queue(QueueOperation::GRAPHICS);
			presentQueue = rRHI->get_queue(QueueOperation::PRESENT);

			initialize_buffers();
			initialize_pipeline();
		}

		void initialize_buffers()
		{
			auto& resources = rRHI->get_resource_handler();

			{
				BufferDescriptor descriptor = {
					.type = BufferType::VERTEX_BUFFER | BufferType::TRANSFER_DST,
				};

				std::array<Vertex, 4> vertices = {
					Vertex{glm::vec3{-0.5f, -0.5f, 0.0f}},
					Vertex{glm::vec3{0.5f, -0.5f, 0.0f}},
					Vertex{glm::vec3{0.5f, 0.5f, 0.0f}},
					Vertex{glm::vec3{-0.5f, 0.5f, 0.0f}},
				};

				vbo = resources.create_empty_buffer(descriptor, sizeof(vertices) * 4);
				resources.update_buffer(vbo, vertices.data(), {.size = sizeof(vertices)});
			}
		}

		void initialize_pipeline()
		{
			// clang-format off
			auto& resources = rRHI->get_resource_handler();

			// Shaders:
			shader_handle vertexShader =
				resources.create_shader({"res/shaders/basic/basic_vert.spv", ShaderType::VERTEX});
			shader_handle fragmentShader =
				resources.create_shader({"res/shaders/basic/basic_frag.spv", ShaderType::FRAGMENT});

			constexpr auto VertexBindings = std::to_array<BindingDescriptor>
			({
				{
					.binding = 0, 
					.stride = sizeof(Vertex), 
					.inputRate = InputRate::VERTEX
				}
			});

			constexpr auto Attributes = std::to_array<Attribute>
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

				.rasterizerStage = 
				{
					.frontFace = FrontFace::COUNTER_CLOCKWISE, 
					.cullMode = CullMode::FRONT
				},
				.viewports = Viewports,
				.scissors = Scissors,
				.dynamicStates = DynamicStates,
				/*.descriptorSetLayout =
				{
					.entries = std::to_array<const DescriptorSetLayoutEntry>({
					{
						.binding = 0,
						.amount = 1,
						.type = DescriptorType::UNIFORM_BUFFER,
						.stage = PipelineStage::VERTEX_STAGE
					}
				})},*/

				.vertexShader = vertexShader,
				.fragmentShader = fragmentShader,
			};

			pipeline = resources.create_graphics_pipeline(pipelineDescriptor);
			// clang-format on
		}

	private:
		NON_OWNING RenderInterface* rRHI = nullptr;
		CommandBuffer mCommandBuffer;

	private:
		buffer_handle vbo;
		pipeline_handle pipeline;
		queue_handle graphicsQueue, presentQueue;
	};
}