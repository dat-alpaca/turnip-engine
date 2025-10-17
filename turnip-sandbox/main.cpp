#include <turnip_engine.hpp>

using namespace tur;

class MainView : public View
{
public:
	void on_engine_startup() override { TUR_LOG_DEBUG("Engine Startup"); }
	void on_engine_shutdown() override { TUR_LOG_DEBUG("Engine Shutdown"); }

	void on_view_added() override
	{
		// Scene creation:
		engine->get_scene_holder().create_scene(true);
		mCurrentScene = engine->get_scene_holder().get_current_scene();

		// Camera:
		{
			mCamera.set_position(glm::vec3(0.0f));
			mCamera.set_target(glm::vec3(0.0f, 0.0f, -1.0f));
		}

		// Renderer Systems:
		auto& rhi = engine->get_render_interface();
		mQuadSystem.initialize(&rhi, mCurrentScene, &mCamera);

		// Scene population:
		{
			mEntity = mCurrentScene->add_entity("test");
			mScript = mEntity.add_component<ScriptComponent>(mEntity, "res/player.lua");

			// Textures:
			mEntity.add_component<Sprite2DComponent>();

			// Textures:
			engine->get_asset_library().load_texture_imm(
				"res/textures/face.png", mEntity.get_component<Sprite2DComponent>().textureHandle
			);

			// Transform:
			TransformComponent transformComponent;
			transformComponent.transform.position = glm::vec3(0.f);
			transformComponent.transform.scale = glm::vec3(100.0f, 100.f, 1.f);
			mEntity.add_component<TransformComponent>(transformComponent);
		}

		// ScriptSystem:
		mScriptSystem.initialize(mCurrentScene);
		mScriptSystem.wake();
	}

	void on_render() override { mQuadSystem.render(); }
	void on_render_gui() override {}

	void on_update() override
	{
		mCamera.update_view();
		mQuadSystem.update();
		mScriptSystem.update();
	}
	void on_event(tur::Event& event) override
	{
		mQuadSystem.on_event(event);

		Subscriber subscriber(event);
		subscriber.subscribe<WindowResizeEvent>(
			[&](const WindowResizeEvent& resizeEvent) -> bool
			{
				mCamera.set_orthogonal(
					0.0f, static_cast<f32>(resizeEvent.width), 0.0f, static_cast<f32>(resizeEvent.height)
				);
				return false;
			}
		);
	}

private:
	Scene* mCurrentScene;
	ImmQuadSystem mQuadSystem;

	ScriptSystem mScriptSystem;
	ScriptComponent mScript;
	Entity mEntity;

	Camera mCamera;
};

int main()
{
	TurnipEngine engine("res/engine_config.json");
	engine.add_view(tur::make_unique<MainView>());

	engine.start();

	return 0;
}