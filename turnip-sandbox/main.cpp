#include <turnip_engine.hpp>

using namespace tur;

class MainView : public View
{
public:
	void on_engine_startup() override {}
	void on_engine_shutdown() override { TUR_LOG_DEBUG("Engine Shutdown"); }

	void on_view_added() override
	{
		mRenderer.initialize(&engine->get_render_interface());

		// Scene creation:
		engine->get_scene_holder().create_scene(true);
		mCurrentScene = engine->get_scene_holder().get_current_scene();

		// Textures:
		engine->get_asset_library().load_texture("res/textures/face.png");

		// Scene population:
		mEntity = mCurrentScene->add_entity("test");
		mScript = mEntity.add_component<ScriptComponent>(mEntity, "res/player.lua");
		mEntity.add_component<TransformComponent>();

		// Running script (on_wake):
		mScript.script.call("on_wake");
	}

	void on_view_removed() override { TUR_LOG_DEBUG("MainView Removed"); }

	void on_render() override { mRenderer.render(); }
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

	ImmQuadRenderer mRenderer;
};

int main()
{
	TurnipEngine engine("res/engine_config.json");
	engine.add_view(tur::make_unique<MainView>());

	engine.start();

	return 0;
}