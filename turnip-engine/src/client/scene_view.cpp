#include "scene_view.hpp"
#include "engine/turnip_engine.hpp"

namespace tur
{
	void SceneView::wake_scene()
	{
		scriptSystem.wake_up();
		physicsSystem.wake();
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
		scenegraphSystem.initialize(mSceneHolder.get_current_scene());

		// Physics system:
		physicsSystem.initialize(mSceneHolder.get_current_scene(), &engine->get_physics_handler());
		physicsSystem.set_event_callback(engine->get_event_callback());
		
		// Scripts subsystem:
		scriptSystem.initialize(mSceneHolder.get_current_scene(), &engine->get_script_handler());
		
		// Physics-Script Agent:
		mPhysicsScriptAgent.initialize(mSceneHolder.get_current_scene(), &physicsSystem, &scriptSystem);

		// Binders:
		mTextureAssetBinder.initialize(mSceneHolder.get_current_scene());
		mTextureAgent.initialize(&engine->get_render_interface(), &engine->get_asset_library());
	}
	void SceneView::on_update()
	{
		mainCamera.update_view();
		quadSystem.update();

		physicsSystem.update();
		scriptSystem.on_update();
		scenegraphSystem.update();
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
				scenegraphSystem.set_scene(viewSwitch.currentScene);
				scriptSystem.set_scene(viewSwitch.currentScene);
				quadSystem.set_scene(viewSwitch.currentScene);
				mTextureAssetBinder.set_scene(viewSwitch.currentScene);
				physicsSystem.set_scene(viewSwitch.currentScene);
				mPhysicsScriptAgent.set_scene(viewSwitch.currentScene);

				return false;
			}
		);

		mPhysicsScriptAgent.on_event(event);
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
