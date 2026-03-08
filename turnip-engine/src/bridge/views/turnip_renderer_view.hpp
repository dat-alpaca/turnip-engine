#pragma once
#include "assets/asset_library.hpp"
#include "graphics/renderer/turnip/turnip_renderer.hpp"

#include "event/events.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"
#include "view/view.hpp"

#include "graphics/renderer/turnip/common.hpp"

namespace tur
{
	class TurnipRendererView : public View
	{
	public:
		TurnipRendererView(NON_OWNING RenderInterface* rhi, NON_OWNING AssetLibrary* library)
		{
			rRHI = rhi;
			rLibrary = library;
			mRenderer.initialize(rhi);
		}

	public:
		void render_deferred()
		{ 
			mRenderer.render(); 
		}

		void dispatch()
		{
			mRenderer.dispatch();
		}
	
	public:
		void on_event(Event& event) override
		{
			Subscriber subscriber(event);
			subscriber.subscribe<WindowFramebufferEvent>(
				[&](const WindowFramebufferEvent& resizeEvent) -> bool
				{
					mRenderer.set_viewport({0.f, 0.f, (f32)resizeEvent.width, (f32)resizeEvent.height});
					mRenderer.set_scissor({0, 0, resizeEvent.width, resizeEvent.height});

					return false;
				}
			);

			subscriber.subscribe<SceneSwitchedEvent>([this](const SceneSwitchedEvent& event) -> bool {
				rScene = event.currentScene;
				on_scene_switch();
				return false;
			});

			subscriber.subscribe<CameraSwitchedEvent>([this](const CameraSwitchedEvent& event) -> bool {
				mRenderer.set_camera(event.camera);
				return false;
			});
		}

		void on_update(const Time&) override
		{
			if(!rScene)
				return;

			/* CPU Culling */
			auto cullView = rScene->get_registry().view<const TransformComponent, const CullingComponent, const MeshComponent>();
			for (auto [entity, transform, culling, mesh] : cullView.each())
			{
				bool visible = true /* frustrum_culling() */;
				if(!visible)
					return;

				if(mMeshBatch.find(mesh.meshUUID) != mMeshBatch.end())
				{
					++mMeshBatch[mesh.meshUUID].instanceCount;
					return;
				}

				auto& offset = mOffsets[mesh.assetHandle];
				DrawCommand drawCommand;
				{
					drawCommand.count 			= mesh.indexCount;
					drawCommand.instanceCount 	= 1;
					drawCommand.firstIndex 		= offset.firstIndex;
					drawCommand.baseVertex 		= offset.baseVertex;
					drawCommand.baseInstance 	= 0;
				}

				mMeshBatch[mesh.meshUUID] = (drawCommand);
			}

			mDrawCommands.clear();
			mDrawCommands.reserve(mMeshBatch.size());
			for(auto [_, drawCommand] : mMeshBatch)
				mDrawCommands.push_back(drawCommand);

			/* Uploading commands */

		}

		void on_render() override
		{
			mRenderer.render();
		}

	private:
		void on_scene_switch()
		{
			mRenderer.clear_buffers();

			auto cullView = rScene->get_registry().view<const MeshComponent>();
			for (auto [entity, mesh] : cullView.each())
			{
				auto& meshAsset = rLibrary->get_mesh_asset(mesh.assetHandle);
				auto offset = mRenderer.upload_mesh(meshAsset.meshData.indices, meshAsset.meshData.vertices);
				
				mOffsets[mesh.assetHandle] = offset;
			}
		}

	public:
		TurnipRenderer& renderer() { return mRenderer; }

	private:
        NON_OWNING Scene* rScene = nullptr;
		TurnipRenderer mRenderer;
	
	private:
		std::unordered_map<asset_handle, OffsetData> mOffsets;
		std::unordered_map<uuid, DrawCommand> mMeshBatch;
		std::vector<DrawCommand> mDrawCommands;

		NON_OWNING RenderInterface* rRHI = nullptr;
		NON_OWNING AssetLibrary* rLibrary = nullptr;
	};
}