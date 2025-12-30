#pragma once
#include "scene/scene.hpp"

namespace tur
{
	class ScenegraphSystem
	{
	public:
		void initialize(NON_OWNING Scene* scene) { set_scene(scene); }
		void set_scene(NON_OWNING Scene* scene) { rScene = scene; }

	public:
		void on_update()
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
