#pragma once
#include "client/system/tilemap_system.hpp"
#include "scene/scene_holder.hpp"
#include "view/view.hpp"

#include "system/imm_quad_system.hpp"
#include "system/physics_system.hpp"
#include "system/scenegraph_system.hpp"
#include "system/script_system.hpp"
#include "system/texture_asset_binder_system.hpp"
#include "system/tilemap_system.hpp"

#include "agents/physics_script_agent.hpp"
#include "agents/texture_asset_agent.hpp"

namespace tur
{
	class SceneView : virtual public View
	{
	public:
		void wake_scene();

	public:
		void on_view_added() override;
		void on_update(const Time& time) override;
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

		// TODO: add scene removal
		// void remove_scene(scene_handle sceneHandle) { mSceneHolder.remove_scene(sceneHandle); }
		void switch_current_scene(scene_handle sceneHandle) { mSceneHolder.set_current_scene(sceneHandle); }

	public:
		ImmQuadRenderer& get_imm_renderer() { return quadSystem.renderer(); }
		Scene& get_current_scene() { return *mSceneHolder.get_current_scene(); }

	protected:
		ScenegraphSystem scenegraphSystem;
		ScriptSystem scriptSystem;
		ImmQuadSystem quadSystem;
		TilemapSystem tilemapSystem;
		Camera mainCamera;
		PhysicsSystem physicsSystem;

	private:
		TextureAssetBinderSystem mTextureAssetBinder;
		TextureAssetBinderAgent mTextureAgent;
		PhysicsScriptAgent mPhysicsScriptAgent;
		SceneHolder mSceneHolder;
	};
}