#pragma once
#include "scene/scene_holder.hpp"
#include "system/imm_quad_system.hpp"
#include "system/script_system.hpp"
#include "view/view.hpp"

namespace tur
{
	class SceneView : virtual public View
	{
	public:
		void wake_scene();

	public:
		virtual void on_view_added() override;
		virtual void on_update() override;
		virtual void on_render() override;
		virtual void on_event(Event& event) override;

	public:
		virtual void on_engine_startup(){};
		virtual void on_engine_shutdown(){};
		virtual void on_view_removed(){};
		virtual void on_render_gui(){};

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
		ScriptSystem scriptSystem;
		ImmQuadSystem quadSystem;
		Camera mainCamera;

	private:
		SceneHolder mSceneHolder;
	};
}