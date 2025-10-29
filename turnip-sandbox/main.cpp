#include <turnip_engine.hpp>

using namespace tur;

class MainSceneView final : public SceneView
{
public:
	void on_view_added() final
	{
		SceneView::on_view_added();
		initialize_entities();

		wake_scene();
	}

private:
	void initialize_entities()
	{
		mEntity = get_current_scene().add_entity("bloky");
		auto script = mEntity.add_component<ScriptComponent>(mEntity, "res/player.lua");

		// Textures:
		mEntity.add_component<Sprite2DComponent>();

		engine->get_asset_library().load_texture_imm(
			"res/textures/face.png", mEntity.get_component<Sprite2DComponent>().textureHandle
		);

		// Transform:
		TransformComponent transformComponent;
		{
			transformComponent.transform.position = glm::vec3(0.f);
			transformComponent.transform.scale = glm::vec3(100.0f, 100.f, 1.f);
		}
		mEntity.add_component<TransformComponent>(transformComponent);
	}

private:
	Entity mEntity;
};

int main()
{
	TurnipEngine engine("res/engine_config.json");
	engine.add_view(tur::make_unique<MainSceneView>());
	engine.start();

	return 0;
}