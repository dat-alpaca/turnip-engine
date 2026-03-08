#pragma once
#include "assets/asset.hpp"
#include "assets/asset_library.hpp"
#include "event/asset_events/mesh_uploaded_event.hpp"
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
				on_scene_switched();
				return false;
			});

			subscriber.subscribe<MeshUploadedEvent>([this](const MeshUploadedEvent& event) -> bool {
				upload_mesh_data_on_load(event);
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
				if(!mesh.indexCount)
					continue;
				
				bool visible = true /* frustrum_culling() */;
				if(!visible)
					continue;

				if(mMeshBatch.find(mesh.meshUUID) != mMeshBatch.end())
				{
					++mMeshBatch[mesh.meshUUID].instanceCount;
					continue;
				}
			}

			mDrawCommands.clear();
			mDrawCommands.reserve(mMeshBatch.size());
			for(auto& [_, drawCommand] : mMeshBatch)
			{
				mDrawCommands.push_back(drawCommand);
				drawCommand.instanceCount = 0;
			}

			/* Uploading commands */
			mRenderer.set_draw_commands(mDrawCommands);
		}

		void on_render() override
		{
			mRenderer.render();
		}

	private:
		void on_scene_switched()
		{
			mRenderer.clear_buffers();

			auto cullView = rScene->get_registry().view<const MeshComponent>();
			for (auto [entity, mesh] : cullView.each())
			{
				auto& meshAsset = rLibrary->get_mesh_asset(mesh.assetHandle);
				if(meshAsset.state == AssetState::LOADING)
					continue;

				auto offset = mRenderer.upload_mesh(meshAsset.meshData.indices, meshAsset.meshData.vertices);
				mOffsets[mesh.assetHandle] = offset;

				DrawCommand drawCommand;
				{
					drawCommand.count 			= mesh.indexCount;
					drawCommand.instanceCount 	= 0;
					drawCommand.firstIndex 		= offset.firstIndex;
					drawCommand.baseVertex 		= offset.baseVertex;
					drawCommand.baseInstance 	= 0;
				}

				mMeshBatch[mesh.meshUUID] = drawCommand;
			}
		}

		void upload_mesh_data_on_load(const MeshUploadedEvent& event)
		{
			auto view = rScene->get_registry().view<const MeshComponent>();
			for (auto [entity, mesh] : view.each())
			{
				if(mesh.assetHandle != event.data.meshAssetHandle)
					continue;

				auto& meshAsset = rLibrary->get_mesh_asset(mesh.assetHandle);
				TUR_ASSERT(meshAsset.state == AssetState::READY, "Upload mesh event signaled before mesh marked as ready.");
					
				auto offset = mRenderer.upload_mesh(meshAsset.meshData.vertices, meshAsset.meshData.indices);				
				mOffsets[mesh.assetHandle] = offset;

				DrawCommand drawCommand;
				{
					drawCommand.count 			= mesh.indexCount;
					drawCommand.instanceCount 	= 0;
					drawCommand.firstIndex 		= offset.firstIndex;
					drawCommand.baseVertex 		= offset.baseVertex;
					drawCommand.baseInstance 	= 0;
				}

				mMeshBatch[mesh.meshUUID] = drawCommand;
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