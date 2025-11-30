#include "scene.hpp"
#include "components.hpp"
#include "entity.hpp"

namespace tur
{
	Entity Scene::add_entity()
	{
		return add_entity({}, {});
	}
	Entity Scene::add_entity(const std::string& entityName)
	{
		return add_entity({}, entityName);
	}
	Entity Scene::add_entity(UUID uuid, const std::string& entityName, entt::entity parent)
	{
		Entity entity = {mRegistry.create(), this};

		// UUID:
		UUIDComponent& uuidComp = entity.add_component<UUIDComponent>(uuid);

		// Name:
		NameComponent& nameComp = entity.add_component<NameComponent>();
		{
			nameComp.name = entityName.empty() ? "Entity #" + std::to_string(mDiagnostics.entityCount) : entityName;
		}

		// Hierarchy:
		HierarchyComponent& hierarchyComp = entity.add_component<HierarchyComponent>();
		{
			hierarchyComp.level = 0;
		}

		// Record:
		++mDiagnostics.entityCount;

		return entity;
	}

	void Scene::remove_entity(Entity entity)
	{
		mRegistry.destroy(entity);
	}
}