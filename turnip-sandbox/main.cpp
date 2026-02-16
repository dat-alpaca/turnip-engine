#include "assets/font/font_asset.hpp"
#include "client/views/text_view.hpp"
#include <turnip_engine.hpp>
#include <ft2build.h>
#include <freetype/freetype.h>

using namespace tur;

class MainSceneView final : public View
{
public:
	void on_view_added() final
	{
		initialize_views();

		// Windowing:
		// Warning: Required for Wayland. For some reason, it does not send a resize event on window creation.
		WindowFramebufferEvent initialSizeEvent(engine->get_window_dimensions().x, engine->get_window_dimensions().y);
		engine->get_event_callback()(initialSizeEvent);

		mProject.initialize("project");

		initialize_resources();
		initialize_entities();

		// Windowing:
		// Warning: Required for Wayland. For some reason, it does not send a resize event on window creation.
		WindowFramebufferEvent initialSizeEvent0(engine->get_window_dimensions().x, engine->get_window_dimensions().y);
		engine->get_event_callback()(initialSizeEvent0);
	}

	void on_render() final
	{
		auto& rhi = engine->get_render_interface();

		CommandBuffer commandBuffer = rhi.create_command_buffer();
		commandBuffer.reset(rhi.get_current_command_buffer());
		commandBuffer.set_clear_color({}, ClearFlags::COLOR | ClearFlags::DEPTH);

		commandBuffer.begin();
		commandBuffer.begin_rendering();

		rCubemapView->render_deferred();
		rQuadView->render_deferred();
		rTilemapView->render_deferred();
		rMeshView->render_deferred();
		rFontView->render_deferred();

		std::vector<CommandBuffer::BufferType> commandBuffers;
		{
			commandBuffers.reserve(4);

			if (rCubemapView->renderer().is_valid())
				commandBuffers.push_back(rCubemapView->renderer().get_command_buffer().get_buffer());

			commandBuffers.push_back(rQuadView->renderer().get_command_buffer().get_buffer());

			if (!rTilemapView->renderer().is_empty())
				commandBuffers.push_back(rTilemapView->renderer().get_command_buffer().get_buffer());

			if (rMeshView->renderer().mesh_count() > 0)
				commandBuffers.push_back(rMeshView->renderer().get_command_buffer().get_buffer());

			if (!rFontView->renderer().is_empty())
				commandBuffers.push_back(rFontView->renderer().get_command_buffer().get_buffer());
		}

		commandBuffer.execute_secondary_buffers(std::span{commandBuffers.data(), commandBuffers.size()});

		commandBuffer.end_rendering();
		commandBuffer.blit();
		commandBuffer.end();
	}

private:
	void initialize_views()
	{
		auto& views = engine->get_view_handler();
		auto& rhi = engine->get_render_interface();
		auto& library = engine->get_asset_library();
		auto& physics = engine->get_physics_handler();
		auto& scriptHandler = engine->get_script_handler();

		// Final touches:
		engine->get_asset_library().create_default_texture();
		engine->get_script_handler().initialize_scene_holder(&mSceneHolder);

		mSceneHolder.set_event_callback(engine->get_event_callback());

		// Views:
		assetBinderView 	= engine->add_view(tur::make_unique<AssetBinderView>(&rhi, &library));

		immQuadView 		= engine->add_view(tur::make_unique<ImmQuadView>(&rhi));
		cubemapView 		= engine->add_view(tur::make_unique<CubemapView>(&rhi));
		tilemapView 		= engine->add_view(tur::make_unique<TilemapView>(&rhi));
		fontView 			= engine->add_view(tur::make_unique<TextView>(&rhi, &library));
		meshView 			= engine->add_view(tur::make_unique<MeshView>(&rhi));
		scenegraphView 		= engine->add_view(tur::make_unique<ScenegraphView>());
		cameraView 			= engine->add_view(tur::make_unique<CameraView>());
		sceneSignalView 	= engine->add_view(tur::make_unique<SceneSignalView>(&library));

		physicsView 		= engine->add_view(tur::make_unique<PhysicsView>(&physics));
		scriptView			= engine->add_view(tur::make_unique<ScriptView>(&scriptHandler));

		auto* physicsPtr 	= views.get<PhysicsView>(physicsView);
		auto* scriptPtr 	= views.get<ScriptView>(scriptView);
		physicsScriptView 	= engine->add_view(tur::make_unique<PhysicsScriptView>(physicsPtr, scriptPtr));

		// Handles:
		rQuadView 		= views.get<ImmQuadView>(immQuadView);
		rCubemapView 	= views.get<CubemapView>(cubemapView);
		rMeshView 		= views.get<MeshView>(meshView);
		rTilemapView 	= views.get<TilemapView>(tilemapView);
		rFontView		= views.get<TextView>(fontView);
	}

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

		for(const FontAsset& font : mProject.get_fonts())
			library.load_font_async(mProject.get_project_filepath(font.metadata.filepath), font.metadata, font.options, font.height, font.isSDF);
	}

	void initialize_entities()
	{
		bool isMainSet = false;
		for(const auto& sceneFilepath : mProject.get_scene_filepaths())
		{
			scene_handle sceneHandle = mSceneHolder.create_scene();
			Scene* scene = mSceneHolder.get_scene(sceneHandle);
			engine->load_scene(sceneFilepath, mProject, scene);
			mSceneHolder.set_current_scene(sceneHandle);
			return;
		}
	}

private:
	SceneHolder mSceneHolder;
	Project mProject;

private:
	NON_OWNING ImmQuadView* rQuadView 		= nullptr;
	NON_OWNING CubemapView* rCubemapView 	= nullptr;
	NON_OWNING MeshView* 	rMeshView 		= nullptr;
	NON_OWNING TilemapView* rTilemapView 	= nullptr;
	NON_OWNING TextView* 	rFontView		= nullptr;

private:
	view_handle assetBinderView 	= invalid_handle;
	view_handle immQuadView 		= invalid_handle;
	view_handle cubemapView 		= invalid_handle;
	view_handle tilemapView			= invalid_handle;
	view_handle meshView 			= invalid_handle;
	view_handle scenegraphView 		= invalid_handle;
	view_handle cameraView			= invalid_handle;
	view_handle sceneSignalView 	= invalid_handle;
	view_handle physicsView			= invalid_handle;
	view_handle scriptView			= invalid_handle;
	view_handle physicsScriptView   = invalid_handle;
	view_handle fontView			= invalid_handle;
};

int main()
{
	TurnipEngine engine("res/engine_config.json");
	engine.add_view(tur::make_unique<MainSceneView>());
	engine.start();

	return 0;
}
