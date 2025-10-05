#include <turnip_engine.hpp>

using namespace tur;

class MainView : public View
{
public:
	void on_engine_startup() override {}
	void on_engine_shutdown() override { TUR_LOG_DEBUG("Engine Shutdown"); }

	void on_view_added() override
	{
		// Graphics:
		{
			auto& rhi = engine->get_render_interface();
			auto& resources = rhi.get_resource_handler();

			{
				mCommandBuffer = rhi.create_command_buffer();

				graphicsQueue = rhi.get_queue(QueueOperation::GRAPHICS);
				presentQueue = rhi.get_queue(QueueOperation::PRESENT);

				// Vertex Binding:
				VertexInputDescriptor vertexInput;
				{
					vertexInput.vertexBindings;
					vertexInput.attributes;
				}

				// Descriptor Layout:
				DescriptorSetLayout layout{
					.entries = std::to_array<const DescriptorSetLayoutEntry>({{.binding = 0,
																			   .amount = 1,
																			   .type = DescriptorType::UNIFORM_BUFFER,
																			   .stage = PipelineStage::VERTEX_STAGE}})};

				// Shaders:
				shader_handle vertexShader =
					resources.create_shader({"res/shaders/basic/basic_vert.spv", ShaderType::VERTEX});
				shader_handle fragmentShader =
					resources.create_shader({"res/shaders/basic/basic_frag.spv", ShaderType::FRAGMENT});

				// Pipeline:
				// TODO: fix clang-format
				PipelineDescriptor pipelineDescriptor = {.vertexInputStage = vertexInput,
														 .vertexShader = vertexShader,
														 .fragmentShader = fragmentShader,
														 .dynamicStates = std::to_array<const DynamicState>(
															 {DynamicState::VIEWPORT, DynamicState::SCISSOR})};

				pipeline = resources.create_graphics_pipeline(pipelineDescriptor);

				// Main Render Target:
				RenderTargetDescriptor descriptor;
				{
					descriptor.attachmentDescriptions.push_back({});
					descriptor.attachmentDescriptions.push_back({});
				}
			}
		}

		// Scene creation:
		engine->get_scene_holder().create_scene(true);
		mCurrentScene = engine->get_scene_holder().get_current_scene();

		// Scene population:
		mEntity = mCurrentScene->add_entity("test");
		mScript = mEntity.add_component<ScriptComponent>(mEntity, "res/player.lua");
		mEntity.add_component<TransformComponent>();

		// Running script (on_wake):
		mScript.script.call("on_wake");
	}

	void on_view_removed() override { TUR_LOG_DEBUG("MainView Removed"); }

	void on_render() override
	{
		auto& rhi = engine->get_render_interface();

		rhi.begin_frame();
		mCommandBuffer.reset(rhi.get_current_command_buffer());
		mCommandBuffer.set_clear_color(ClearColor(color::Blue));

		{
			mCommandBuffer.begin();

			// bind_resources()
			// draw()

			mCommandBuffer.end();
		}

		rhi.end_frame();
		rhi.submit(graphicsQueue);
		rhi.present(presentQueue);
	}
	void on_render_gui() override {}

	void on_update() override
	{
		mScript.script.environment["on_update"]();

		for (auto& view : engine->get_view_system().get_views())
		{
			for (auto [entity, script] : mCurrentScene->get_registry().view<ScriptComponent>().each())
			{
				script.script.call("on_update");
			}
		}
	}
	void on_event(tur::Event& event) override {}

private:
	ScriptComponent mScript;
	Scene* mCurrentScene;
	Entity mEntity;

	CommandBuffer mCommandBuffer;

	pipeline_handle pipeline;
	queue_handle graphicsQueue, presentQueue;
};

int main()
{
	TurnipEngine engine("res/engine_config.json");
	engine.add_view(tur::make_unique<MainView>());

	engine.start();

	return 0;
}