#include "scene_view.hpp"
#include "client/agents/physics_script_agent.hpp"
#include "engine/turnip_engine.hpp"
#include "event/window_events/window_framebuffer_event.hpp"
#include "graphics/types/clear_color.hpp"
#include "utils/color.hpp"
#include "vulkan/objects/vulkan_command_buffer.hpp"
#include "vulkan/vulkan.hpp"
#include <vector>

namespace tur
{
	void SceneView::wake_scene()
	{
		scriptSystem.wake_up();
		mPhysicsScriptAgent.on_wake();
	}

	void SceneView::on_view_added()
	{
		// Default assets:
		engine->get_asset_library().create_default_texture();

		// Event callbacks:
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

		// Scene Signals:
		signalSystem.initialize(&engine->get_asset_library());

		// Windowing:
		// Warning: Required for Wayland. For some reason, it does not send a resize event on window creation.
		WindowFramebufferEvent initialSizeEvent(engine->get_window_dimensions().x, engine->get_window_dimensions().y);
		engine->get_event_callback()(initialSizeEvent);
	}
	void SceneView::on_update(const Time& time)
	{
		mainCamera.update_view();
		quadSystem.update();
		tilemapSystem.update(time);

		scriptSystem.on_update(time);
		physicsSystem.update();
		scenegraphSystem.update();
	}
	void SceneView::on_render()
	{
		auto& rhi = engine->get_render_interface();

		CommandBuffer commandBuffer = rhi.create_command_buffer();
		commandBuffer.reset(rhi.get_current_command_buffer());

		tilemapSystem.render();
		quadSystem.render();

		commandBuffer.begin();
		commandBuffer.begin_rendering();

			std::vector<vk::CommandBuffer> commandBuffers;
			{
				commandBuffers.reserve(2);

				commandBuffers.push_back(
					quadSystem.renderer().get_command_buffer().get_command_buffer()
				);

				if(!tilemapSystem.renderer().is_empty())
					commandBuffers.push_back(tilemapSystem.renderer().get_command_buffer().get_command_buffer());
			}

			commandBuffer.execute_secondary_buffers(std::span{commandBuffers.data(), commandBuffers.size()});
		
		commandBuffer.end_rendering();
		commandBuffer.blit();
		commandBuffer.end();
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
				tilemapSystem.set_scene(viewSwitch.currentScene);
				mTextureAssetBinder.set_scene(viewSwitch.currentScene);
				physicsSystem.set_scene(viewSwitch.currentScene);
				mPhysicsScriptAgent.set_scene(viewSwitch.currentScene);

				return false;
			}
		);

		signalSystem.on_event(event);
		mPhysicsScriptAgent.on_event(event);
		mTextureAgent.on_event(event);
		mTextureAssetBinder.on_event(event);
		quadSystem.on_event(event);
		tilemapSystem.on_event(event);
	}

	void SceneView::initialize_renderers()
	{
		// Renderer Systems:
		auto& rhi = engine->get_render_interface();
		quadSystem.initialize(&rhi, mSceneHolder.get_current_scene(), &mainCamera);
		quadSystem.renderer().set_clear_color(ClearColor(color::Black), ClearFlags::COLOR);
	
		// Tilemap:
		tilemapSystem.initialize(&rhi, mSceneHolder.get_current_scene(), &mainCamera);
		tilemapSystem.renderer().set_clear_color(ClearColor(color::Black), ClearFlags::COLOR);
	}
}
