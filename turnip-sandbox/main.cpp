#include "assets/texture/texture_asset.hpp"
#include "client/project/project.hpp"
#include "client/scene_serialization.hpp"
#include "client/scene_view.hpp"
#include "event/subscriber.hpp"
#include "graphics/components.hpp"
#include "graphics/tile.hpp"
#include "physics/physics_components.hpp"

#include <turnip_engine.hpp>

using namespace tur;

class MainSceneView final : public SceneView
{
public:
	void on_view_added() final
	{
		SceneView::on_view_added();
		mProject.initialize("project");

		initialize_resources();
		initialize_entities();
		wake_scene();
	}

private:
	void initialize_resources()
	{
		AssetLibrary& library = engine->get_asset_library();
		for(const TextureAsset& texture : mProject.get_textures())
			library.load_texture_async(mProject.get_project_filepath(texture.metadata.filepath), texture.options);

		for(const AudioAsset& audio : mProject.get_audios())
			library.load_audio_async(mProject.get_project_filepath(audio.metadata.filepath));
	}

	void initialize_entities()
	{
		AssetLibrary& library = engine->get_asset_library();

		deserialize_scene(&get_current_scene(), mProject, "main_scene.scene");

		SceneDeserializedEvent deserializeEvent(&get_current_scene(), mProject);
		engine->get_event_callback()(deserializeEvent);
		return;
		
		// Entity 0:
		auto entity = get_current_scene().add_entity("bloky");
		{
			auto script = entity.add_component<ScriptComponent>("res/player.lua");

			entity.add_component<Sprite2DComponent>(library.get_asset_handle("res/textures/face.png"));
			entity.add_component<AudioSourceComponent>(library.get_asset_handle("res/audio/sound.wav"));

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
		}

		// Entity 1:
		{
			auto entity0 = get_current_scene().add_entity("newone");

			entity0.add_component<Sprite2DComponent>(library.get_asset_handle("res/textures/face.png"));

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

			floor.add_component<Sprite2DComponent>(library.get_asset_handle("res/textures/face.png"));

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

			auto& tilemap = floor.add_component<Tilemap2DComponent>(library.get_asset_handle("res/textures/face_sheet.png"), 32);
			
			TilemapChunk chunk;
			{
				for(u8 x = 0; x < 3; ++x)
				{
					for(u8 y = 0; y < 3; ++y)
					{
						TileFlags flags;
						flags.set_enable(true);
						flags.set_flip(false);
						flags.set_animation_size(5);

						chunk.chunks.push_back(Tile{{x, y}, 0, flags});
					}
				}
			}
			
			tilemap.worldData.push_back(chunk);
		}

		mProject.add_scene(&get_current_scene(), "main_scene.scene");
		mProject.save();
	}

	void on_event(Event& event) override
	{
		SceneView::on_event(event);

		Subscriber subscriber(event);
		subscriber.subscribe<WindowFramebufferEvent>(
			[&](const WindowFramebufferEvent& resizeEvent) -> bool
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
	static constexpr inline float PixelPerMeter = 32.f;
	Project mProject;
};

int main()
{
	TurnipEngine engine("res/engine_config.json");
	engine.add_view(tur::make_unique<MainSceneView>());
	engine.start();

	return 0;
}