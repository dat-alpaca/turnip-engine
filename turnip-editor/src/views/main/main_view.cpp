#include "main_view.hpp"
#include "event/events.hpp"

#include <core/scene/scene_serialization.hpp>

using namespace tur;

MainView::MainView(const ProjectData& projectData)
	: m_ProjectData(projectData)
{
	m_MainMenuBar.initialize(this);
}

void MainView::set_project_data(const ProjectData& projectData)
{
	// Reset:
	scene.get_registry().clear();
	m_SceneData.viewerSelectedEntity = Entity();

	// TODO: record scene data inside projectData.
	// Deserialize main scene:
	if (!std::filesystem::is_regular_file(projectData.projectPath / "scene.json"))
		return;

	SceneDeserializer deserializer(&scene, projectData.projectPath / "scene.json");
	deserializer.deserialize();

	// Project Data:
	{
		m_ProjectData = projectData;
		load_project_data(m_ProjectData, &engine->assetLibrary, &engine->workerPool, get_main_event_callback());

		ScriptHandler::set_project(m_ProjectData);
	}
}

void MainView::on_view_added()
{
	m_EntityInspector.set_callback(std::move(get_main_event_callback()));
	m_SceneEditor.set_callback(std::move(get_main_event_callback()));
	m_MainMenuBar.set_callback(std::move(get_main_event_callback()));
	m_SceneViewer.set_callback(std::move(get_main_event_callback()));
	m_AssetLibraryEditor.set_callback(std::move(get_main_event_callback()));

	set_project_data(m_ProjectData);

	// Widgets:
	m_EntityInspector.initialize(engine, &scene, &m_SceneData);
	m_SceneViewer.initialize(&scene, &m_SceneData);
	m_SceneEditor.initialize(&engine->graphicsDevice, engine->guiSystem.get(), &m_SceneData);
	m_AssetLibraryEditor.initialize(&engine->assetLibrary, &engine->rendererAssemblerSystem, engine->guiSystem.get());

	// Main Menu:
	{
		m_MainMenuBar.register_widget(&m_EntityInspector, "Entity Inspector");
		m_MainMenuBar.register_widget(&m_SceneViewer, "Scene Viewer");
		m_MainMenuBar.register_widget(&m_SceneEditor, "Scene Editor");
		m_MainMenuBar.register_widget(&m_AssetLibraryEditor, "Asset Library");
	}

	initialize_renderer_system();
	initialize_textures();
}
void MainView::on_update()
{
	scene.on_update_runtime();

	if (m_UpdateRenderCalled)
	{
		auto size = m_SceneEditor.get_size();
		update_render_target(static_cast<u32>(size.x), static_cast<u32>(size.y));
		m_UpdateRenderCalled = false;
	}
}
void MainView::on_render_gui()
{
	ImGui::DockSpaceOverViewport();

	m_MainMenuBar.on_render();

	{
		ImGui::Begin("Runtime");

		if (ImGui::Button("Start"))
			scene.start_runtime();

		ImGui::SameLine();

		if (ImGui::Button("Stop"))
			scene.stop_runtime();

		ImGui::End();
	}

	m_SceneEditor.on_render_gui();
	m_SceneViewer.on_render_gui();
	m_EntityInspector.on_render_gui();
	m_AssetLibraryEditor.on_render_gui();
}
void MainView::on_event(Event& event)
{
	Subscriber subscriber(event);

	subscriber.subscribe<OnProjectEdited>(
		[&](OnProjectEdited&) -> bool
		{
			append_window_title(m_ProjectData.projectName + "*");
			return false;
		}
	);

	subscriber.subscribe<OnProjectSaved>(
		[&](OnProjectSaved&) -> bool
		{
			SceneSerializer serializer(&scene, m_ProjectData.projectPath / "scene.json");
			serializer.serialize();

			save_project_data(m_ProjectData, &engine->assetLibrary);

			append_window_title(m_ProjectData.projectName);
			return false;
		}
	);

	subscriber.subscribe<OnEntityInspectorInitialize>(
		[&](const OnEntityInspectorInitialize&) -> bool
		{
			TUR_LOG_INFO("Inspector initialized");
			return false;
		}
	);

	subscriber.subscribe<SceneEditorResized>(
		[&](const SceneEditorResized& resizeEvent) -> bool
		{
			if (resizeEvent.width <= 0 || resizeEvent.height <= 0)
				return false;

			// Render target:
			call_update_render();

			// Viewport:
			if (r_QuadRenderer)
				renderer_set_viewport(
					r_QuadRenderer, {0.0f, 0.0f, (float)resizeEvent.width, (float)resizeEvent.height}
				);

			// Camera:
			m_SceneData.editorCamera.camera.set_orthogonal(
				0.f, (float)resizeEvent.width, 0.f, (float)resizeEvent.height, -1.f, 1.f
			);
			return false;
		}
	);

	subscriber.subscribe<SceneEditorClicked>(
		[&](const SceneEditorClicked& clickEvent) -> bool
		{
			// Select object on view:
			return false;
		}
	);

	m_EntityInspector.on_event(event);
	m_SceneEditor.on_event(event);
}
void MainView::on_render()
{
	quad_renderer_system_begin(engine->quadRendererSystem, m_SceneData.sceneRenderTarget);
	quad_renderer_system_render(engine->quadRendererSystem);
	quad_renderer_system_end(engine->quadRendererSystem);

	// instanced_quad_system_begin(engine->instancedQuadSystem);
	// instanced_quad_system_render(engine->instancedQuadSystem, m_SceneData.sceneRenderTarget);
	// instanced_quad_system_end(engine->instancedQuadSystem);
}

void MainView::initialize_textures()
{
	auto& assetLibrary = engine->assetLibrary;

	// Scene Texture:
	update_render_target(100, 100);

	// Default Texture:
	{
		auto [descriptor, asset] = create_default_texture();
		auto defaultTexture = engine->graphicsDevice.create_texture(descriptor, asset);

		quad_renderer_set_default_texture(engine->quadRendererSystem.renderer, defaultTexture);
	}
}
void MainView::initialize_renderer_system()
{
	r_InstancedRenderer = &engine->instancedQuadSystem.renderer;
	r_QuadRenderer = &engine->quadRendererSystem.renderer;

	const Color& color = {40, 40, 40, 255};

	// Main:
	{
		quad_renderer_system_set_scene(engine->quadRendererSystem, &scene);
		quad_renderer_system_set_camera(engine->quadRendererSystem, &m_SceneData.editorCamera.camera);
		renderer_set_clear_color(r_QuadRenderer, color);
	}

	// Instanced:
	{
		auto windowSize = engine->window.data.properties.dimensions;

		instanced_quad_system_set_scene(engine->instancedQuadSystem, &scene);
		instanced_quad_system_set_camera(engine->instancedQuadSystem, &m_SceneData.editorCamera.camera);

		renderer_set_clear_color(r_InstancedRenderer, color);
		renderer_set_viewport(r_InstancedRenderer, {0.f, 0.f, (float)windowSize.x, (float)windowSize.y});
	}

	// GUI:
	{
		engine->guiSystem->set_clear_color(color);
	}

	set_renderer_assembler_system_scene(engine->rendererAssemblerSystem, &scene);
}

void MainView::append_window_title(const std::string& extraText)
{
	auto appendedTitle = engine->configData.windowProperties.title + " - " + extraText;
	set_window_title(&engine->window, appendedTitle);
}
void MainView::call_update_render()
{
	m_UpdateRenderCalled = true;
}
void MainView::update_render_target(u32 width, u32 height)
{
	auto& graphicsDevice = engine->graphicsDevice;
	auto& gui = engine->guiSystem;

	// Render target texture:
	{
		TextureDescriptor textureDescriptor;
		textureDescriptor.width = width;
		textureDescriptor.height = height;

		if (m_SceneData.sceneTexture != invalid_handle)
		{
			// TODO: [vulkan] update gui descriptor set
			graphicsDevice.destroy_texture(m_SceneData.sceneTexture);
			graphicsDevice.destroy_render_target(m_SceneData.sceneRenderTarget);
		}

		m_SceneData.sceneTexture = graphicsDevice.create_texture(textureDescriptor);

		RenderTargetDescriptor renderDescriptor;
		{
			renderDescriptor.colorAttachments.push_back(m_SceneData.sceneTexture);
			renderDescriptor.width = width;
			renderDescriptor.height = height;
		}
		m_SceneData.sceneRenderTarget = graphicsDevice.create_render_target(renderDescriptor);
	}
}
