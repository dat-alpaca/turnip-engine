#pragma once
#include "utils/transform.hpp"
#include "utils/uuid/uuid.hpp"
#include <string>

namespace tur
{
	struct UUIDComponent
	{
	public:
		UUIDComponent() = default;
		UUIDComponent(UUID uuid)
			: uuid(uuid)
		{
		}
		UUIDComponent(const UUIDComponent&) = default;

	public:
		UUID uuid;
	};

	struct NameComponent
	{
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
	};
}