#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace tur
{
	struct Transform
	{
	public:
		Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
			: position(position),
			  rotation(rotation),
			  scale(scale)
		{
		}

		Transform(const glm::mat4& transform)
			: mTransform(transform)
		{
		}

		Transform(const Transform& transform)
			: mTransform(transform.mTransform),
			  position(transform.position),
			  rotation(transform.rotation),
			  scale(transform.scale)
		{
		}

		Transform() = default;

	public:
		Transform operator*(Transform& rhs)
		{
			Transform transform(*this);
			transform.mTransform = rhs.transform() * transform.transform();

			transform.decompose_transform();
			return transform;
		}

	public:
		glm::mat4& transform()
		{
			glm::mat4 model(1.0f);
			model = glm::translate(model, position);

			glm::quat quaternion = glm::quat(glm::radians(rotation));
			model *= glm::mat4_cast(quaternion);

			model = glm::scale(model, scale);

			mTransform = model;
			return mTransform;
		}

		glm::mat4& raw_transform() { return mTransform; }

		void decompose_transform()
		{
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::quat quaternion;
			glm::decompose(mTransform, scale, quaternion, position, skew, perspective);
			rotation = glm::degrees(glm::eulerAngles(quaternion));
		}

	public:
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 rotation = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);

	private:
		glm::mat4 mTransform = glm::mat4(1.0f);
	};
}