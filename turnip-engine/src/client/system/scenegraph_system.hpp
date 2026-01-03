#pragma once
#include "scene/scene.hpp"
#include "scene/components.hpp"
#include "client/system/system.hpp"

namespace tur
{
	class ScenegraphSystem : public System
	{
	public:
		void initialize(NON_OWNING Scene* scene) { set_scene(scene); }

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
	};
}
