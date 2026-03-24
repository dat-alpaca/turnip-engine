#pragma once
#include "assets/asset.hpp"
#include "assets/asset_library.hpp"
#include "defines.hpp"
#include "entt/entity/fwd.hpp"
#include "event/asset_events/mesh_uploaded_event.hpp"
#include "graphics/renderer/turnip/turnip_renderer.hpp"

#include "event/events.hpp"
#include "logging/logging.hpp"
#include "scene/components.hpp"
#include "scene/components/graphics.hpp"
#include "scene/scene.hpp"
#include "utils/uuid/uuid.hpp"
#include "view/view.hpp"

#include "graphics/renderer/turnip/common.hpp"
#include <unordered_set>

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
			mObjectData.clear();
			auto cullView = rScene->get_registry().view<const TransformComponent, const CullingComponent, const MeshComponent, const MaterialComponent>();
			for (auto [e, transform, culling, mesh, material] : cullView.each())
			{
				if(!mesh.indexCount)
					continue;
				
				bool visible = true /* frustrum_culling() */;
				if(!visible)
					continue;

				if(mMeshBatch.find(mesh.meshUUID) == mMeshBatch.end())
					continue;

				++mMeshBatch[mesh.meshUUID].instanceCount;
				
				// TODO: unordered_map<uuid> -> vector of object data?
				mObjectData[mesh.meshUUID].push_back
				({
					.model = transform.worldTransform.to_matrix(),
					.material = material.materialHandle
				});
			}

			u32 objectIndexCurrentPosition = 0;
			mDrawCommands.clear();
			mDrawCommands.reserve(mMeshBatch.size());
			mRenderer.clear_all_objects();

			struct Info
			{
				uuid uuid = invalid_uuid;
				DrawCommand drawCommand;
			};
			std::unordered_map<material_handle, Info> infoMap;
			for(auto& [uuid, drawCommand] : mMeshBatch)
			{
				if(mObjectData.find(uuid) == mObjectData.end())
					TUR_LOG_CRITICAL("Sometimes brains are really stupid, don't mind this codebase's stupidity please.");

				for(const auto& object : mObjectData[uuid])
				{
					infoMap[object.material].drawCommand = drawCommand;
					infoMap[object.material].uuid = uuid; // an extra assignment never hurt anyone
				}
			}

			for(auto& [material, info] : infoMap)
			{
				if(mObjectData.find(info.uuid) == mObjectData.end())
					TUR_LOG_CRITICAL("Sometimes brains are really stupid, don't mind this codebase's stupidity please.");

				const auto& objectData = mObjectData[info.uuid];

				info.drawCommand.baseInstance = objectIndexCurrentPosition;
				mDrawCommands.push_back(info.drawCommand);

				for(const auto& object : objectData)
					mRenderer.push_object(material, object);

				info.drawCommand.instanceCount = 0;
				objectIndexCurrentPosition += objectData.size();

				/* Uploading commands */
				mRenderer.set_draw_commands(material, mDrawCommands);
			}
		}

		void on_render() override
		{
			mRenderer.render();
		}

	private:
		void on_scene_switched()
		{
			mRenderer.clear_all_buffers();
			mRenderer.clear_all_objects();

			auto view = rScene->get_registry().view<const MeshComponent, const MaterialComponent>();
			for (auto [entity, mesh, material] : view.each())
			{
				auto& meshAsset = rLibrary->get_mesh_asset(mesh.assetHandle);
				if(meshAsset.state == AssetState::LOADING)
					continue;

				auto offset = mRenderer.upload_mesh
				(
					material.materialHandle, 
					meshAsset.meshData.indices, 
					meshAsset.meshData.vertices
				);

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
			auto view = rScene->get_registry().view<const MeshComponent, const MaterialComponent>();
			for (auto [entity, mesh, material] : view.each())
			{
				if(mesh.assetHandle != event.data.meshAssetHandle)
					continue;

				auto& meshAsset = rLibrary->get_mesh_asset(mesh.assetHandle);
				TUR_ASSERT(meshAsset.state == AssetState::READY, "Upload mesh event signaled before mesh marked as ready.");
				
				if(mMeshBatch.find(mesh.meshUUID) != mMeshBatch.end())
					continue;

				auto offset = mRenderer.upload_mesh
				(
					material.materialHandle,
					meshAsset.meshData.vertices,
					meshAsset.meshData.indices
				);				

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
		std::unordered_map<uuid, std::vector<TurnipRenderer::ObjectData>> mObjectData;
		std::unordered_map<uuid, DrawCommand> mMeshBatch;
		std::vector<DrawCommand> mDrawCommands;

		NON_OWNING RenderInterface* rRHI = nullptr;
		NON_OWNING AssetLibrary* rLibrary = nullptr;
	};
}