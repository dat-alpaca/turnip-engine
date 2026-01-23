#include "assets/asset.hpp"
#include "assets/cubemap/cubemap_asset.hpp"
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
			library.load_texture_async(mProject.get_project_filepath(texture.metadata.filepath), texture.metadata, texture.options);

		for(const AudioAsset& audio : mProject.get_audios())
			library.load_audio_async(mProject.get_project_filepath(audio.metadata.filepath), audio.metadata);

		for(const MeshAsset& mesh : mProject.get_meshes())
			library.load_mesh_async(mProject.get_project_filepath(mesh.metadata.filepath), mesh.metadata);

		for(const CubemapAsset& cubemap : mProject.get_cubemaps())
		{
			std::array<std::filesystem::path, 6> filepaths;
			for(u64 i = 0; i < 6; ++i)
				filepaths[i] = mProject.get_project_filepath(cubemap.filepaths[i]);

			library.load_cubemap_async(filepaths, cubemap);
		}
	}

	void initialize_entities()
	{
		bool isMainSet = false;
		for(const auto& sceneFilepath : mProject.get_scene_filepaths())
		{
			if(!isMainSet)
			{
				deserialize_scene(&get_current_scene(), mProject, sceneFilepath);
				
				SceneDeserializedEvent deserializeEvent(&get_current_scene(), mProject);
				engine->get_event_callback()(deserializeEvent);
				
				isMainSet = true;
				continue;
			}

			scene_handle sceneHandle = add_scene();
			Scene& scene = get_scene(sceneHandle);
			deserialize_scene(&scene, mProject, sceneFilepath);

			SceneDeserializedEvent deserializeEvent(&scene, mProject);
			engine->get_event_callback()(deserializeEvent);
		}
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