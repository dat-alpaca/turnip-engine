#include "assets/texture/texture_options.hpp"
#include "client/project/project.hpp"
#include "client/scene_serialization.hpp"
#include "client/scene_view.hpp"
#include "event/subscriber.hpp"
#include "graphics/components.hpp"
#include "physics/physics_components.hpp"
#include <turnip_engine.hpp>

using namespace tur;

class MainSceneView final : public SceneView
{
public:
	void on_view_added() final
	{
		SceneView::on_view_added();
		mProject.initialize("res/project");
		mainCamera.set_orthogonal(0.0f, 600.0f, 0, 600.f);

		initialize_resources();
		initialize_entities();
		wake_scene();
	}

private:
	void initialize_resources()
	{
		AssetLibrary& library = engine->get_asset_library();

		mFaceAsset = library.load_texture_async("res/textures/face.png", TextureOptions{.isTextureArray=false});
		mSoundAsset = library.load_audio_async("res/audio/sound.wav");
		mFloorAsset = library.load_texture_async("res/textures/floor.png", TextureOptions{.isTextureArray=false});
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
			transformComponent.transform.position = glm::vec3(3.0f, 1.0f, 0.0f);
			transformComponent.transform.scale = glm::vec3(1.f, 1.f, 1.f);
		}
		entity.add_component<TransformComponent>(transformComponent);

		entity.add_component<Body2DComponent>(BodyType::DYNAMIC);
		entity.add_component<RectCollider2D>(1.f, 1.f);

		auto& scene = get_current_scene(); 
		
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

		// Floor:
		{
			auto floor = get_current_scene().add_entity("floor");

			floor.add_component<Sprite2DComponent>(mFloorAsset);

			// Transform:
			TransformComponent transformComponent0;
			{
				transformComponent0.transform.position = glm::vec3(3.0, 5.f, 0.0f);
				transformComponent0.transform.scale = glm::vec3(1.f, 1.0f, 1.f);
			}
			floor.add_component<TransformComponent>(transformComponent0);

			floor.add_component<Body2DComponent>();
			floor.add_component<RectCollider2D>(1.f, 1.f);
		}

		// Big Floor:
		{
			auto floor = get_current_scene().add_entity("floor");

			auto& tilemap = floor.add_component<Tilemap2DComponent>(mFloorAsset, 32);
			tilemap.assetHandle = mFloorAsset;
			TilemapChunk chunk;
			{
				for(size_t x = 0; x < 1; ++x)
				{
					for(size_t y = 0; y < 1; ++y)
						chunk.chunks.push_back(Tile{{x, y}, (u32)(x + y) / 100, 0});						
				}
			}
			
			tilemap.worldData.push_back(chunk);
		}

		mProject.add_scene(&scene, "main_scene.json");
		mProject.save();
	}

	void on_event(Event& event) override
	{
		SceneView::on_event(event);

		Subscriber subscriber(event);
		subscriber.subscribe<WindowResizeEvent>(
			[&](const WindowResizeEvent& resizeEvent) -> bool
			{
				auto dimensions = engine->get_window_dimensions();

				mainCamera.set_orthogonal(
					0.0f, 
					static_cast<f32>(dimensions.x / PixelPerMeter), 
					0.0f, 
					static_cast<f32>(dimensions.y / PixelPerMeter)
				);
				return false;
			}
		);
	}

private:
	static constexpr inline float PixelPerMeter = 100.f;

	asset_handle mSoundAsset;
	asset_handle mFaceAsset;
	asset_handle mFloorAsset;
	Project mProject;
};

int main()
{
	TurnipEngine engine("res/engine_config.json");
	engine.add_view(tur::make_unique<MainSceneView>());
	engine.start();

	return 0;
}