#pragma once
#include <box2d/box2d.h>
#include "assert/assert.hpp"
#include "view/view.hpp"
#include "entt/entity/fwd.hpp"
#include "event/events.hpp"
#include "scene/components.hpp"

#include "event/physics_events/contact_event.hpp"
#include "event/scene_events/scene_switched_event.hpp"
#include "event/subscriber.hpp"
#include "memory/memory.hpp"
#include "physics/physics_components.hpp"
#include "physics/physics_handler.hpp"

#include "scene/scene.hpp"

#include <unordered_map>

inline bool operator==(const b2ShapeId& lhs, const b2ShapeId& rhs)
{
	return lhs.index1 == rhs.index1 && lhs.world0 == rhs.world0 && lhs.generation == rhs.generation;
}

namespace tur
{
	struct ShapeIdHash
	{
		std::size_t operator()(const b2ShapeId& id) const
		{
			return std::hash<int32_t>{}(id.index1) ^ (std::hash<uint16_t>{}(id.generation) << 1);
		}
	};
}

namespace tur
{
	class PhysicsView : public View
	{
	public:
		PhysicsView(NON_OWNING PhysicsHandler* physicsHandler)
		{
			rPhysicsHandler = physicsHandler;
		}

	public:
		void on_event(Event& event)
		{
			Subscriber subscriber(event);

			subscriber.subscribe<SceneCreatedEvent>([this](const SceneCreatedEvent& event) -> bool {
				auto& registry = event.scene->get_registry();
				registry.on_construct<Body2DComponent>().connect<&PhysicsView::on_body2d_added>(this);
				registry.on_construct<RectCollider2D>().connect<&PhysicsView::on_rect2d_added>(this);

				return false;
			});

			subscriber.subscribe<SceneSwitchedEvent>([this](const SceneSwitchedEvent& event) -> bool {
				rScene = event.currentScene;
				
				// Destroy previous scene bodies and rects.
				destroy_physics_components_for_scene(event.previousScene);

				// Create current scene bodies and rects.
				create_physics_components_for_scene(event.currentScene);

				return false;
			});
		}

		void on_fixed_update()
		{
			if (mEnableTransformSync)
				synchronize_physics_world();

			rPhysicsHandler->step();
			process_contact_events(rPhysicsHandler->get_contact_events());

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

		void process_contact_events(b2ContactEvents events)
		{
			// Begin:
			for (int i = 0; i < events.beginCount; ++i)
			{
				b2ContactBeginTouchEvent* beginEvent = &events.beginEvents[i];
				auto entityAHandle = mShapeMap.at(beginEvent->shapeIdA);
				auto entityBHandle = mShapeMap.at(beginEvent->shapeIdB);

				OnContactEvent contactEvent(ContactType::BEGIN, entityAHandle, entityBHandle);
				callback(contactEvent);
			}

			// End:
			for (int i = 0; i < events.endCount; ++i)
			{
				b2ContactEndTouchEvent* endEvent = &events.endEvents[i];
				auto entityAHandle = mShapeMap.at(endEvent->shapeIdA);
				auto entityBHandle = mShapeMap.at(endEvent->shapeIdB);

				OnContactEvent contactEvent(ContactType::END, entityAHandle, entityBHandle);
				callback(contactEvent);
			}

			// Hit:
			for (int i = 0; i < events.hitCount; ++i)
			{
				b2ContactHitEvent* hitEvent = &events.hitEvents[i];
				auto entityAHandle = mShapeMap.at(hitEvent->shapeIdA);
				auto entityBHandle = mShapeMap.at(hitEvent->shapeIdB);

				OnContactEvent contactEvent(ContactType::HIT, entityAHandle, entityBHandle, hitEvent->approachSpeed);
				callback(contactEvent);
			}
		}

	private:
		void on_body2d_added(entt::registry& registry, entt::entity entity)
		{
			auto& body = registry.get<Body2DComponent>(entity);
			const auto& transform = registry.get<TransformComponent>(entity).transform;
			
			// Create body:
			body.bodyDef = b2DefaultBodyDef();

			// Set properties:
			body.bodyDef.type = static_cast<b2BodyType>(body.type);
			body.bodyDef.isBullet = body.isBullet;

			// Set position:
			const auto& worldPos = transform.position;
			body.bodyDef.position = {worldPos.x, worldPos.y};

			// Note: Creation is deferred.
		}

		void on_rect2d_added(entt::registry& registry, entt::entity entity) 
		{
			const auto& body = registry.get<Body2DComponent>(entity);
			auto& rectCollider = registry.get<RectCollider2D>(entity);

			// Create polygon:
			rectCollider.polygon = b2MakeBox(rectCollider.width / 2.f, rectCollider.height / 2.f);
			rectCollider.shapeDef = b2DefaultShapeDef();
			rectCollider.shapeDef.density = rectCollider.density;
			rectCollider.shapeDef.material.restitution = rectCollider.restitution;
			rectCollider.shapeDef.material.rollingResistance = rectCollider.rollingResistance;
			rectCollider.shapeDef.material.friction = rectCollider.friction;

			rectCollider.shapeDef.enableContactEvents = true;
			rectCollider.shapeDef.filter.categoryBits = 0x0001;
			rectCollider.shapeDef.filter.maskBits = 0xFFFF;

			// Note: creation is deferred.
		}

		void create_physics_components_for_scene(Scene* scene)
		{
			TUR_ASS(scene);

			auto bodyView = scene->get_registry().view<Body2DComponent>();
			for(auto entity : bodyView)
			{
				auto& body = bodyView.get<Body2DComponent>(entity);
				body.bodyID = rPhysicsHandler->create_body(&body.bodyDef);
			}

			auto rectView = scene->get_registry().view<Body2DComponent, RectCollider2D>();
			for(auto entity : rectView)
			{
				auto& body = bodyView.get<Body2DComponent>(entity);
				auto& collider = rectView.get<RectCollider2D>(entity);
				collider.shapeID = rPhysicsHandler->create_shape(body.bodyID, &collider.shapeDef, &collider.polygon);
				mShapeMap[collider.shapeID] = entity;
			}
		}

		void destroy_physics_components_for_scene(Scene* scene)
		{
			if(!scene)
				return;

			auto bodyView = scene->get_registry().view<Body2DComponent>();
			for(auto entity : bodyView)
			{
				auto& body = bodyView.get<Body2DComponent>(entity);
				rPhysicsHandler->destroy_body(body.bodyID);
			}
		}

	private:
		NON_OWNING PhysicsHandler* rPhysicsHandler = nullptr;
		NON_OWNING Scene* rScene = nullptr;

	private:
		std::unordered_map<b2ShapeId, entt::entity, ShapeIdHash> mShapeMap;
		bool mEnableTransformSync = true;
		bool mIsPrimaryWorld = true;
	};
}