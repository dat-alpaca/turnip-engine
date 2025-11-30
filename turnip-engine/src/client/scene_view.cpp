#include "scene_view.hpp"
#include "engine/turnip_engine.hpp"

namespace tur
{
	void SceneView::wake_scene()
	{
		scriptSubsystem.wake();
	}

	void SceneView::on_view_added()
	{
		mSceneHolder.set_event_callback(engine->get_event_callback());
		mTextureAgent.set_event_callback(engine->get_event_callback());
		add_scene(true);

		// Camera:
		{
			mainCamera.set_position(glm::vec3(0.0f));
			mainCamera.set_target(glm::vec3(0.0f, 0.0f, -1.0f));
		}

		// Renderers:
		initialize_renderers();

		// Scenegraph:
		scenegraphSubsystem.initialize(mSceneHolder.get_current_scene());

		// Physics system:
		physicsSystem.initialize(mSceneHolder.get_current_scene(), &engine->get_physics_handler());

		// Scripts subsystem:
		scriptSubsystem.initialize(mSceneHolder.get_current_scene(), &engine->get_script_system());

		// Binders:
		mTextureAssetBinder.initialize(mSceneHolder.get_current_scene());
		mTextureAgent.initialize(&engine->get_render_interface(), &engine->get_asset_library());
	}
	void SceneView::on_update()
	{
		scenegraphSubsystem.update();

		mainCamera.update_view();
		quadSystem.update();

		scriptSubsystem.on_update();
	}
	void SceneView::on_render()
	{
		quadSystem.render();
	}
	void SceneView::on_event(Event& event)
	{
		Subscriber subscriber(event);
		subscriber.subscribe<ViewSwitchedEvent>(
			[&](const ViewSwitchedEvent& viewSwitch) -> bool
			{
				scenegraphSubsystem.set_scene(viewSwitch.currentScene);
				scriptSubsystem.set_scene(viewSwitch.currentScene);
				quadSystem.set_scene(viewSwitch.currentScene);
				mTextureAssetBinder.set_scene(viewSwitch.currentScene);
				physicsSystem.set_scene(viewSwitch.currentScene);

				return false;
			}
		);

		subscriber.subscribe<WindowResizeEvent>(
			[&](const WindowResizeEvent& resizeEvent) -> bool
			{
				mainCamera.set_orthogonal(
					0.0f, static_cast<f32>(resizeEvent.width), 0.0f, static_cast<f32>(resizeEvent.height)
				);
				return false;
			}
		);

		mTextureAgent.on_event(event);
		mTextureAssetBinder.on_event(event);
		quadSystem.on_event(event);
	}

	void SceneView::initialize_renderers()
	{
		// Renderer Systems:
		auto& rhi = engine->get_render_interface();
		quadSystem.initialize(&rhi, mSceneHolder.get_current_scene(), &mainCamera);
		quadSystem.renderer().set_clear_color(ClearColor(color::Black), ClearFlags::COLOR);
	}
}
