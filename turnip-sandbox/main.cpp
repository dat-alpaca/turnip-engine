#include <turnip_engine.hpp>

using namespace tur;

class MainView : public View
{
public:
	void on_engine_startup() override {}
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
			mCamera.set_orthogonal(0.0f, 640.0f, 0.0f, 480.f);
		}

		// Renderer:
		mQuadRenderer.initialize(&engine->get_render_interface(), mCurrentScene, &mCamera);

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

		// Running script (on_wake):
		mScript.script.call("on_wake");
	}

	void on_view_removed() override { TUR_LOG_DEBUG("MainView Removed"); }

	void on_render() override { mQuadRenderer.render(); }
	void on_render_gui() override {}

	void on_update() override
	{
		mCamera.update_view();
		mQuadRenderer.update();

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

	Camera mCamera;
	ImmQuadSystem mQuadRenderer;
};

int main()
{
	TurnipEngine engine("res/engine_config.json");
	engine.add_view(tur::make_unique<MainView>());

	engine.start();

	return 0;
}