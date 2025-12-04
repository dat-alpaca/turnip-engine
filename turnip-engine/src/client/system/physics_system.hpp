#pragma once
#include "physics/physics_handler.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"
#include <cmath>

namespace tur
{
	class PhysicsSystem
	{
	public:
		void initialize(NON_OWNING Scene* scene, NON_OWNING PhysicsHandler* physicsHandler)
		{
			rPhysicsHandler = physicsHandler;
			set_scene(scene);
		}
		void set_scene(NON_OWNING Scene* scene) { rScene = scene; }

	public:
		void wake()
		{
			auto view0 = rScene->get_registry().view<Body2DComponent, TransformComponent>();
			for (auto [e, body, transformComp] : view0.each())
			{
				const auto& worldPos = transformComp.transform.position;
				body.bodyDef.position = {worldPos.x, worldPos.y};
			}

			auto view1 = rScene->get_registry().view<Body2DComponent>();
			for (auto entity : view1)
			{
				auto& body = view1.get<Body2DComponent>(entity);
				body.bodyID = b2CreateBody(rPhysicsHandler->get_world_id(), &body.bodyDef);
			}

			auto view2 = rScene->get_registry().view<Body2DComponent, RectCollider2D>();
			for (auto [e, body, rectCollider] : view2.each())
				b2CreatePolygonShape(body.bodyID, &rectCollider.shape, &rectCollider.polygon);
		}

		void update()
		{
			if (mEnableTransformSync)
				synchronize_physics_world();

			rPhysicsHandler->step();

			synchronize_transform();
		}

	private:
		void synchronize_physics_world()
		{
			auto view = rScene->get_registry().view<TransformComponent, Body2DComponent>();
			for (auto entity : view)
			{
				// TODO: check if transform is dirty. reset velocities.

				auto& transform = view.get<TransformComponent>(entity).transform;
				auto& body = view.get<Body2DComponent>(entity);

				b2Vec2 position = {transform.position.x, transform.position.y};

				float angle = glm::radians(transform.rotation.z);
				b2Rot rotation = b2Rot{cosf(angle), sinf(angle)};

				b2Body_SetTransform(body.bodyID, position, rotation);
			}
		}

		void synchronize_transform()
		{
			auto view = rScene->get_registry().view<TransformComponent, Body2DComponent>();
			for (auto entity : view)
			{
				auto& transform = view.get<TransformComponent>(entity).transform;
				auto& body = view.get<Body2DComponent>(entity);

				b2Vec2 position = b2Body_GetPosition(body.bodyID);

				transform.position.x = position.x;
				transform.position.y = position.y;

				b2Rot rotation = b2Body_GetRotation(body.bodyID);
				float angleInRadians = b2Rot_GetAngle(rotation);
				transform.rotation.z = angleInRadians;
			}
		}

	private:
		NON_OWNING PhysicsHandler* rPhysicsHandler = nullptr;
		NON_OWNING Scene* rScene = nullptr;

	private:
		bool mEnableTransformSync = true;
	};
}