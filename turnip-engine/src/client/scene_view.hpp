#pragma once
#include "client/system/scene_signal_system.hpp"
#include "client/system/tilemap_system.hpp"
#include "scene/scene_holder.hpp"
#include "view/view.hpp"

#include "system/imm_quad_system.hpp"
#include "system/physics_system.hpp"
#include "system/scenegraph_system.hpp"
#include "system/script_system.hpp"
#include "system/tilemap_system.hpp"
#include "system/scene_signal_system.hpp"
#include "system/camera_system.hpp"
#include "system/mesh_system.hpp"

#include "agents/physics_script_agent.hpp"
#include "agents/asset_binder_system.hpp"

namespace tur
{
	class SceneView : virtual public View
	{
	public:
		void wake_scene();

	public:
		void on_view_added() override;
		void on_fixed_update() override;
		void on_update(const Time& time) override;
		void on_post_update(const Time& time) override;
		void on_render() override;
		void on_event(Event& event) override;

	public:
		void on_engine_startup() override {};
		void on_engine_shutdown() override {};
		void on_view_removed() override {};
		void on_render_gui() override {};

	private:
		void initialize_renderers();

	public:
		scene_handle add_scene(bool setAsCurrent = false) { return mSceneHolder.create_scene(setAsCurrent); }
		Scene& get_scene(scene_handle sceneHandle) { return *mSceneHolder.get_scene(sceneHandle); }
		Scene& get_current_scene() { return *mSceneHolder.get_current_scene(); }
		void switch_current_scene(scene_handle sceneHandle) { mSceneHolder.set_current_scene(sceneHandle); }

		// TODO: add scene removal
		// void remove_scene(scene_handle sceneHandle) { mSceneHolder.remove_scene(sceneHandle); }

	public:
		ImmQuadRenderer& get_imm_renderer() { return quadSystem.renderer(); }

	protected:
		ScenegraphSystem scenegraphSystem;
		ScriptSystem scriptSystem;
		
		CameraSystem cameraSystem;
		PhysicsSystem physicsSystem;
		SceneSignalSystem signalSystem;
		AssetBinderSystem mAssetBinderSystem;

		ImmQuadSystem quadSystem;
		TilemapSystem tilemapSystem;
		MeshSystem mMeshSystem;

	private:
		PhysicsScriptAgent mPhysicsScriptAgent;
		SceneHolder mSceneHolder;
	};
}