#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace tur
{
	class Camera
	{
	public:
		void set_orthogonal(float left, float right, float bottom, float top, float zNear, float zFar)
		{
			mProjection = glm::ortho(left, right, bottom, top, zNear, zFar);
		}
		void set_perspective(float fov, float aspectRatio, float zNear, float zFar)
		{
			mProjection = glm::perspective(fov, aspectRatio, zNear, zFar);
		}

		void set_position(const glm::vec3& position) { mPosition = position; }
		void set_target(const glm::vec3& target) { mTarget = target; }

		void update_view() { mView = glm::lookAt(mPosition, mTarget, Camera::Up); }

	public:
		glm::mat4 view() { return mView; }
		glm::mat4 projection() { return mProjection; }

		glm::vec3 position() const { return mPosition; }
		glm::vec3 target() const { return mTarget; }

	private:
		static inline const glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.f);
		glm::vec3 mPosition = glm::vec3(0.0f);
		glm::vec3 mTarget = glm::vec3(0.0f);

	private:
		glm::mat4 mProjection = glm::mat4(1.f);
		glm::mat4 mView = glm::mat4(1.f);
	};
}