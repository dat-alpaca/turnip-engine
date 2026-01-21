#include "scene_view.hpp"
#include "client/agents/physics_script_agent.hpp"
#include "engine/turnip_engine.hpp"
#include "event/window_events/window_framebuffer_event.hpp"
#include "graphics/types/clear_color.hpp"
#include "utils/color.hpp"
#include <vector>

namespace tur
{
	void SceneView::wake_scene()
	{
		scriptSystem.wake_up();

		// Windowing:
		// Warning: Required for Wayland. For some reason, it does not send a resize event on window creation.
		WindowFramebufferEvent initialSizeEvent(engine->get_window_dimensions().x, engine->get_window_dimensions().y);
		engine->get_event_callback()(initialSizeEvent);
	}

	void SceneView::on_view_added()
	{
		// Default assets:
		engine->get_asset_library().create_default_texture();

		// Scene Holder:
		engine->get_script_handler().initialize_scene(&mSceneHolder);

		// Event callbacks:
		mSceneHolder.set_event_callback(engine->get_event_callback());
		mAssetBinderSystem.set_event_callback(engine->get_event_callback());
		add_scene(true);

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

		// Camera System:
		cameraSystem.initialize(mSceneHolder.get_current_scene());

		// Binders:
		mAssetBinderSystem.initialize(&engine->get_render_interface(), &engine->get_asset_library());
		mAssetBinderSystem.set_scene(mSceneHolder.get_current_scene());

		// Scene Signals:
		signalSystem.initialize(&engine->get_asset_library());

		// Windowing:
		// Warning: Required for Wayland. For some reason, it does not send a resize event on window creation.
		WindowFramebufferEvent initialSizeEvent(engine->get_window_dimensions().x, engine->get_window_dimensions().y);
		engine->get_event_callback()(initialSizeEvent);
	}
	void SceneView::on_fixed_update()
	{
		physicsSystem.on_fixed_update();
		scriptSystem.on_fixed_update();
	}
	void SceneView::on_update(const Time& time)
	{
		quadSystem.on_update();
		tilemapSystem.on_update(time);
		mMeshSystem.on_update();

		scriptSystem.on_update(time);
		scenegraphSystem.on_update();

		cameraSystem.on_update();
		quadSystem.set_camera(cameraSystem.get_main_camera());
		mMeshSystem.set_camera(cameraSystem.get_main_camera());
		tilemapSystem.set_camera(cameraSystem.get_main_camera());
	}
	void SceneView::on_post_update(const Time& time)
	{
		scriptSystem.on_post_update(time);
	}
	void SceneView::on_render()
	{
		auto& rhi = engine->get_render_interface();

		CommandBuffer commandBuffer = rhi.create_command_buffer();
		commandBuffer.reset(rhi.get_current_command_buffer());
		commandBuffer.set_clear_color({}, ClearFlags::COLOR);

		commandBuffer.begin();
		commandBuffer.begin_rendering();

			tilemapSystem.render();
			quadSystem.render();
			mMeshSystem.render();

			std::vector<CommandBuffer::BufferType> commandBuffers;
			{
				commandBuffers.reserve(2);

				commandBuffers.push_back(
					quadSystem.renderer().get_command_buffer().get_buffer()
				);

				if(!tilemapSystem.renderer().is_empty())
					commandBuffers.push_back(tilemapSystem.renderer().get_command_buffer().get_buffer());

				commandBuffers.push_back(
					mMeshSystem.renderer().get_command_buffer().get_buffer()
				);
			}

			commandBuffer.execute_secondary_buffers(std::span{commandBuffers.data(), commandBuffers.size()});
		
		commandBuffer.end_rendering();
		commandBuffer.blit();
		commandBuffer.end();
	}
	void SceneView::on_event(Event& event)
	{
		Subscriber subscriber(event);
		subscriber.subscribe<SceneSwitchedEvent>(
			[&](const SceneSwitchedEvent& sceneSwitch) -> bool
			{
				scenegraphSystem.set_scene(sceneSwitch.currentScene);
				scriptSystem.set_scene(sceneSwitch.currentScene);

				quadSystem.set_scene(sceneSwitch.currentScene);
				tilemapSystem.set_scene(sceneSwitch.currentScene);
				mMeshSystem.set_scene(sceneSwitch.currentScene);

				mAssetBinderSystem.set_scene(sceneSwitch.currentScene);
				physicsSystem.set_scene(sceneSwitch.currentScene);
				mPhysicsScriptAgent.set_scene(sceneSwitch.currentScene);

				return false;
			}
		);

		signalSystem.on_event(event);
		mPhysicsScriptAgent.on_event(event);
		mAssetBinderSystem.on_event(event);
		quadSystem.on_event(event);
		tilemapSystem.on_event(event);
		mMeshSystem.on_event(event);
		scriptSystem.on_event(event);
	}

	void SceneView::initialize_renderers()
	{
		// Renderer Systems:
		auto& rhi = engine->get_render_interface();
		quadSystem.initialize(&rhi, mSceneHolder.get_current_scene(), nullptr);
		quadSystem.renderer().set_clear_color(ClearColor(color::Black), ClearFlags::COLOR);
	
		// Tilemap:
		tilemapSystem.initialize(&rhi, mSceneHolder.get_current_scene(), nullptr);
		tilemapSystem.renderer().set_clear_color(ClearColor(color::Black), ClearFlags::COLOR);

		// Mesh:
		mMeshSystem.initialize(&rhi, mSceneHolder.get_current_scene(), nullptr);
		mMeshSystem.renderer().set_clear_color(ClearColor(color::Black), ClearFlags::COLOR);
	}
}
