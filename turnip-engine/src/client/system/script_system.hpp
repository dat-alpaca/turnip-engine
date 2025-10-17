#pragma once
#include "scene/components.hpp"
#include "scene/scene.hpp"

namespace tur
{
	class ScriptSystem
	{
	public:
		void initialize(Scene* scene) { rScene = scene; }

	public:
		void wake()
		{
			for (auto [entity, scriptComp] : rScene->get_registry().view<ScriptComponent>().each())
				scriptComp.script.call("on_wake");
		}

		void update()
		{
			for (auto [entity, scriptComp] : rScene->get_registry().view<ScriptComponent>().each())
				scriptComp.script.call("on_update");
		}

	private:
		NON_OWNING Scene* rScene = nullptr;
	};
}