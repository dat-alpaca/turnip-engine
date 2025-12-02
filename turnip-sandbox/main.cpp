#include <turnip_engine.hpp>

using namespace tur;

class MainSceneView final : public SceneView
{
public:
	void on_view_added() final
	{
		SceneView::on_view_added();

		initialize_resources();
		initialize_entities();
		wake_scene();
	}

private:
	void initialize_resources()
	{
		AssetLibrary& library = engine->get_asset_library();

		mFaceAsset = library.load_texture_async("res/textures/face.png");
		mSoundAsset = library.load_audio_async("res/audio/sound.wav");
	}

	void initialize_entities()
	{
		// Entity 0:
		auto entity = get_current_scene().add_entity("bloky");
		auto script = entity.add_component<ScriptComponent>("res/player.lua");

		entity.add_component<Sprite2DComponent>(mFaceAsset);
		entity.add_component<AudioSourceComponent>(mSoundAsset);

		auto windowSize = engine->get_window_dimensions();

		// Transform:
		TransformComponent transformComponent;
		{
			transformComponent.transform.position = glm::vec3(windowSize.x / 2.f, windowSize.y / 2.f, 0.0f);
			transformComponent.transform.scale = glm::vec3(100.f, 100.f, 1.f);
		}
		entity.add_component<TransformComponent>(transformComponent);

		entity.add_component<Body2DComponent>(BodyType::DYNAMIC);
		entity.add_component<RectCollider2D>(1.f, 1.f);

		// Entity 1:
		{
			auto entity0 = get_current_scene().add_entity("newone");

			entity0.add_component<Sprite2DComponent>(mFaceAsset);

			auto& hierarchy = entity0.get_component<HierarchyComponent>();
			hierarchy.level = 1;
			hierarchy.parent = entity.get_handle();

			// Transform:
			TransformComponent transformComponent0;
			{
				transformComponent0.transform.position = glm::vec3(0.5f, 0.5f, -0.1f);
				transformComponent0.transform.scale = glm::vec3(0.5f, 0.5f, 1.f);
			}
			entity0.add_component<TransformComponent>(transformComponent0);
		}
	}

private:
	asset_handle mSoundAsset;
	asset_handle mFaceAsset;
};

int main()
{
	TurnipEngine engine("res/engine_config.json");
	engine.add_view(tur::make_unique<MainSceneView>());
	engine.start();

	return 0;
}