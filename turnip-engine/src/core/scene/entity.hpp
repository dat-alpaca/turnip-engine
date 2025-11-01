#pragma once
#include <entt/entt.hpp>

#include "common.hpp"
#include "common_components.hpp"
#include "scene.hpp"

namespace tur
{
	class Entity
	{
	public:
		Entity() = default;

		Entity(entt::entity entityHandle, Scene* sceneHandle)
			: mEntityHandle(entityHandle),
			  rScene(sceneHandle)
		{
		}

	public:
		template <typename T, typename... Args>
		T& add_component(Args&&... args)
		{
			T& component = rScene->mRegistry.emplace<T>(mEntityHandle, std::forward<Args>(args)...);
			return component;
		}

		template <typename T>
		void remove_component()
		{
			if (has_component<T>())
			{
				TUR_LOG_WARN("Component already added");
				return;
			}

			rScene->mRegistry.remove<T>(mEntityHandle);
		}

		template <typename T>
		const T& get_component() const
		{
			return rScene->mRegistry.get<T>(mEntityHandle);
		}

		template <typename T>
		T& get_component()
		{
			return rScene->mRegistry.get<T>(mEntityHandle);
		}

		template <typename T>
		bool has_component() const
		{
			return rScene->mRegistry.all_of<T>(mEntityHandle);
		}

	public:
		inline bool is_valid() const { return mEntityHandle != entt::null; }
		inline UUID get_uuid() const { return get_component<UUIDComponent>().uuid; }
		inline std::string name() const { return get_component<NameComponent>().name; }

	public:
		operator entt::entity() const { return mEntityHandle; }
		entt::entity get_handle() const { return mEntityHandle; }
		NON_OWNING Scene* get_scene() const { return rScene; }

	private:
		entt::entity mEntityHandle = entt::null;
		NON_OWNING Scene* rScene = nullptr;
	};
}