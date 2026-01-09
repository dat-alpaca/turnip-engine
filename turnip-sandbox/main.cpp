#include "assets/asset.hpp"
#include "assets/mesh/mesh_asset.hpp"
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

		for(const MeshAsset& mesh : mProject.get_meshes())
			library.load_mesh_async(mProject.get_project_filepath(mesh.metadata.filepath));
	}

	void initialize_entities()
	{
		deserialize_scene(&get_current_scene(), mProject, "main_scene.scene");

		SceneDeserializedEvent deserializeEvent(&get_current_scene(), mProject);
		engine->get_event_callback()(deserializeEvent);
	}

private:
	asset_handle meshHandle;
	Project mProject;
};

int main()
{
	TurnipEngine engine("res/engine_config.json");
	engine.add_view(tur::make_unique<MainSceneView>());
	engine.start();

	return 0;
}