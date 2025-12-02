#pragma once
#include "common.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"

namespace tur
{
	class ScenegraphSystem
	{
	public:
		void initialize(NON_OWNING Scene* scene) { set_scene(scene); }
		void set_scene(NON_OWNING Scene* scene) { rScene = scene; }

	public:
		void update()
		{
			// Resize entity level list:
			u64 maxDepth = 0;
			std::vector<std::vector<Entity>> entities;

			auto view0 = rScene->get_registry().view<HierarchyComponent>();
			for (auto [e, hierarchy] : view0.each())
			{
				if (hierarchy.level > maxDepth)
					maxDepth = hierarchy.level;
			}

			entities.resize(maxDepth + 1);

			// Get all entities based on level:
			auto view1 = rScene->get_registry().view<TransformComponent, HierarchyComponent>();
			for (auto [e, transform, hierarchy] : view1.each())
				entities[hierarchy.level].push_back({e, rScene});

			// Iterate through levels:
			u64 level = 0;
			for (auto& levelEntities : entities)
			{
				for (auto& entity : levelEntities)
				{
					auto& transform = entity.get_component<TransformComponent>();
					auto& hierarchy = entity.get_component<HierarchyComponent>();

					if (level != 0)
					{
						Entity parent = Entity{hierarchy.parent, rScene};
						auto& parentTransform = parent.get_component<TransformComponent>();

						transform.worldTransform = parentTransform.worldTransform * transform.transform;
						TUR_LOG_WARN(
							"{} {} {}", transform.worldTransform.position.x, transform.worldTransform.position.y,
							transform.worldTransform.position.z
						);
					}
					else
						transform.worldTransform = transform.transform;
				}
				++level;
			}
		}

	private:
		NON_OWNING Scene* rScene = nullptr;
	};
}
