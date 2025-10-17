#pragma once
#include "imm_quad_renderer.hpp"
#include "rhi/rhi.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"

namespace tur
{
	class ImmQuadSystem
	{
	public:
		void initialize(RenderInterface* rhi, Scene* scene, Camera* camera)
		{
			mQuadRenderer.initialize(rhi);
			mQuadRenderer.set_camera(camera);
			rScene = scene;
		}

		void render() { mQuadRenderer.render(); }

		void update()
		{
			auto view = rScene->get_registry().view<TransformComponent, Sprite2DComponent>();
			for (auto [e, transform, sprite] : view.each())
			{
				if (mHandleCache.find(e) == mHandleCache.end())
				{
					mHandleCache[e] = mQuadRenderer.add_quad(transform.transform, sprite.textureHandle);
					break;
				}

				auto& quad = mQuadRenderer.get_quad(mHandleCache[e]);
				quad.transform = transform.transform;
				quad.textureHandle = sprite.textureHandle;
			}
		}

	private:
		NON_OWNING Scene* rScene = nullptr;
		ImmQuadRenderer mQuadRenderer;
		std::unordered_map<entt::entity, ImmQuadRenderer::quad_handle> mHandleCache;
	};
}