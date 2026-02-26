#pragma once
#include "utils/transform.hpp"
#include "utils/uuid/uuid.hpp"
#include <entt/entt.hpp>
#include <string>

namespace tur
{
	/* Hierarchy */
	struct HierarchyComponent
	{
		REGISTER_COMPONENT(hierarchy);

	public:
		HierarchyComponent() = default;

	public:
		entt::entity parent = entt::null;
		u64 level = 0;
	};

	struct UUIDComponent
	{
		REGISTER_COMPONENT(uuid);

	public:
		UUIDComponent() = default;
		UUIDComponent(uuid uuid)
			: uuid(uuid)
		{
		}
		UUIDComponent(const UUIDComponent&) = default;

	public:
		uuid uuid;
	};

	struct NameComponent
	{
		REGISTER_COMPONENT(name);

	public:
		NameComponent() = default;
		NameComponent(const NameComponent&) = default;
		NameComponent(const std::string& name)
			: name(name)
		{
		}

	public:
		std::string name;
	};

	struct TransformComponent
	{
		REGISTER_COMPONENT(transform);

	public:
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
			: transform(position, rotation, scale)
		{
		}

	public:
		operator Transform() { return transform; }
		operator glm::mat4() { return transform.transform(); }

	public:
		Transform transform;
		FIELD_SET_INTERNALLY Transform worldTransform;
	};
}
