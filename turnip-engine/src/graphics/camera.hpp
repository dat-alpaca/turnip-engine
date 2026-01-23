#pragma once
#include "glm/trigonometric.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace tur
{
	class Camera
	{
		union CameraDimensions;

	public:
		void set_orthogonal(float left, float right, float bottom, float top, float zNear = -1.f, float zFar = 1.f)
		{
			mProjection = glm::ortho(left, right, bottom, top, zNear, zFar);

			mDimensions = {};
			mDimensions.left = left;
			mDimensions.right = right;
			mDimensions.bottom = bottom;
			mDimensions.top = top;
			mDimensions.zNear = zNear;
			mDimensions.zFar = zFar;

			mIsOrthographic = true;
		}
		void set_perspective(float fov, float aspectRatio, float zNear, float zFar)
		{
			mProjection = glm::perspective(glm::radians(fov), aspectRatio, zNear, zFar);

			mDimensions = {};
			mDimensions.fov = fov;
			mDimensions.aspectRatio = aspectRatio;
			mDimensions.zNear = zNear;
			mDimensions.zFar = zFar;

			mIsOrthographic = false;
		}

		void update_view() { mView = glm::lookAt(position, position + target, Camera::Up); }

	public:
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 target = glm::vec3(0.0f);
	
	public:
		bool is_orthographic() { return mIsOrthographic; }
		glm::mat4 view() { return mView; }
		glm::mat4 projection() { return mProjection; }
		CameraDimensions dimensions() const { return mDimensions; }

	private:
		static inline const glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.f);

	private:
		glm::mat4 mProjection = glm::mat4(1.f);
		glm::mat4 mView = glm::mat4(1.f);
		bool mIsOrthographic = false;

	private:
		union CameraDimensions
		{
			struct
			{
				float left;
				float right;
				float bottom;
				float top;
				float zNear;
				float zFar;
			};

			struct
			{
				float fov;
				float aspectRatio;
			};

		} mDimensions;
	};
}