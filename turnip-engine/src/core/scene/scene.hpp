#pragma once
#include <entt/entt.hpp>
#include <optional>

#include "defines.hpp"
#include "utils/uuid/uuid.hpp"

namespace tur
{
	class Entity;

	class Scene
	{
		friend class Entity;

	public:
		Scene() = default;

		public:
		Entity add_entity();
		Entity add_entity(const std::string& entityName);
		Entity add_entity(UUID uuid, const std::string& entityName, entt::entity parent = entt::null);

		void remove_entity(Entity entity);

	public:
		inline u64 get_entity_count() const { return mDiagnostics.entityCount; }
		inline entt::registry& get_registry() { return mRegistry; }
		inline const entt::registry& get_registry() const { return mRegistry; }

	private:
		entt::registry mRegistry;

		struct Diagnostics
		{
			u64 entityCount = 0;
		} mDiagnostics;
	};
}