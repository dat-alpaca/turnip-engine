#pragma one
#include "common.hpp"
#include "event/events.hpp"
#include "scene.hpp"
#include <vector>

namespace tur
{
	using scene_handle = handle_type;

	class SceneHolder
	{
	public:
		void set_event_callback(EventCallback&& callback) { mEventCallback = std::move(callback); }

	public:
		scene_handle create_scene(bool setAsCurrent = false)
		{
			mScenes.push_back(tur::make_unique<Scene>());

			if (setAsCurrent)
				set_current_scene(mScenes.back().get());

			return static_cast<scene_handle>(mScenes.size() - 1);
		}
		void set_current_scene(Scene* scene)
		{
			mCurrentScene = scene;

			ViewSwitchedEvent viewSwitchedEvent(mCurrentScene);
			mEventCallback(viewSwitchedEvent);
		}
		void set_current_scene(scene_handle handle)
		{
			TUR_ASSERT(handle >= 0 && handle < mScenes.size() - 1, "Invalid scene handle provided");
			mCurrentScene = mScenes[handle].get();

			ViewSwitchedEvent viewSwitchedEvent(mCurrentScene);
			mEventCallback(viewSwitchedEvent);
		}

	public:
		NON_OWNING Scene* get_current_scene() const { return mCurrentScene; }
		NON_OWNING Scene* get_scene(scene_handle handle) const
		{
			TUR_ASSERT(handle >= 0 && handle < mScenes.size() - 1, "Invalid scene handle provided");
			return mScenes[handle].get();
		}

	private:
		std::vector<tur_unique<Scene>> mScenes;
		Scene* mCurrentScene = nullptr;
		EventCallback mEventCallback;
	};
}