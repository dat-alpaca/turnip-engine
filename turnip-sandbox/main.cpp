#include "assets/asset.hpp"
#include "graphics/components.hpp"
#include "scene/common_components.hpp"
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

		modelHandle = library.load_model_async("project/models/beretta_m9/beretta.gltf");
		//modelHandle = library.load_model_async("project/models/crate/wooden crate.gltf");
	}

	void initialize_entities()
	{
		deserialize_scene(&get_current_scene(), mProject, "main_scene.scene");

		SceneDeserializedEvent deserializeEvent(&get_current_scene(), mProject);
		engine->get_event_callback()(deserializeEvent);

		auto e = get_current_scene().add_entity();
		e.add_component<MeshComponent>(modelHandle);
		auto& t = e.get_component<TransformComponent>();
		t.transform.position.x = 4.f;
		t.transform.position.y = 4.f;
		t.transform.rotation.y = 90.f;
		t.transform.scale.x = 3.f;
		t.transform.scale.y = 3.f;
		t.transform.scale.z = 3.f;
	}

private:
	asset_handle modelHandle;
	Project mProject;
};

int main()
{
	TurnipEngine engine("res/engine_config.json");
	engine.add_view(tur::make_unique<MainSceneView>());
	engine.start();

	return 0;
}