#pragma once
#include "event/subscriber.hpp"
#include "memory/memory.hpp"
#include "scene/scene.hpp"
#include "scene/components.hpp"
#include "view/view.hpp"
#include "event/events.hpp"

namespace tur
{
	class ScenegraphView : public View
	{
	public:
		void on_event(Event& event) override
		{
			Subscriber subscriber(event);
			subscriber.subscribe<SceneSwitchedEvent>([this](const SceneSwitchedEvent& event) -> bool {
				rScene = event.currentScene;
				return false;
			});
		}

		void on_update(const Time&) override
		{
			auto& registry = rScene->get_registry();
			auto view = registry.view<TransformComponent, const HierarchyComponent>();
			view.use<HierarchyComponent>();

			for (auto [entity, transform, hierarchy] : view.each())
			{
				if (hierarchy.parent != entt::null)
				{
					Entity parent = Entity(hierarchy.parent, rScene);
					auto& parentTransform = parent.get_component<TransformComponent>();

					transform.worldTransform = parentTransform.worldTransform * transform.transform;
				}
				else
					transform.worldTransform = transform.transform;
			}
		}

	private:
		NON_OWNING Scene* rScene = nullptr;
	};
}
