#pragma once
#include "assets/model/model_asset.hpp"
#include "assets/texture/texture_asset.hpp"
#include "assets/texture/texture_options.hpp"
#include "defines.hpp"
#include "event/asset_events/model_uploaded_event.hpp"
#include "graphics/components.hpp"
#include "graphics/objects/buffer.hpp"
#include "graphics/objects/texture.hpp"
#include "assets/asset.hpp"
#include "event/events.hpp"
#include "event_consumer.hpp"
#include "event_emitter.hpp"
#include "rhi/rhi.hpp"

#include "scene/components.hpp"
#include "assets/asset_library.hpp"
#include "client/system/system.hpp"

namespace tur
{
	class AssetBinderSystem : public IEventConsumer, public IEventEmitter, public System
	{
	public:
		void initialize(NON_OWNING RenderInterface* rhi, NON_OWNING AssetLibrary* library)
		{
			rRHI = rhi;
			rLibrary = library;
		}

	public:
		void on_event(Event& event)
		{
			Subscriber subscriber(event);
			subscriber.subscribe<AssetLoadedEvent>(
				[&](const AssetLoadedEvent& assetLoadedEvent) -> bool
				{
					handle_asset_loaded_event(assetLoadedEvent);
					return false;
				}
			);

			subscriber.subscribe<TextureUploadedEvent>(
				[&](const TextureUploadedEvent& textureUploadedEvent) -> bool
				{
					handle_texture_uploaded_event(textureUploadedEvent);
					return false;
				}
			);

			subscriber.subscribe<ModelUploadedEvent>(
				[&](const ModelUploadedEvent& modelUploadedEvent) -> bool
				{
					handle_model_uploaded_event(modelUploadedEvent);
					return false;
				}
			);
		}

	private:
		void handle_asset_loaded_event(const AssetLoadedEvent& event)
		{
			switch(event.type)
			{
				case AssetType::TEXTURE_ARRAY:
				case AssetType::TEXTURE:
				{
					bool isArray = event.type == AssetType::TEXTURE_ARRAY;

					texture_handle textureHandle = create_texture_from_asset(event.assetHandle, isArray);
					TextureUploadedEvent uploadEvent(event.assetHandle, textureHandle, event.type);
					callback(uploadEvent);
					
				} break;

				case AssetType::MODEL:
				{
					auto data = create_model_from_asset(event.assetHandle);
					ModelUploadedEvent uploadEvent(data);
					callback(uploadEvent);
				} break;

				default:
					return;
			}
		}

		void handle_texture_uploaded_event(const TextureUploadedEvent& textureUploaded)
		{
			switch(textureUploaded.type)
			{
				case AssetType::TEXTURE:
					update_texture_handle(textureUploaded);
					break;
					
				case AssetType::TEXTURE_ARRAY:
					update_texture_array_handle(textureUploaded);
					break;
				
				default:
					TUR_LOG_ERROR("Attempted to register an invalid asset type as texture.");
			}
		}

		void handle_model_uploaded_event(const ModelUploadedEvent& modelUploadedEvent)
		{
			update_model_handles(modelUploadedEvent);
		}

	private:
		texture_handle create_texture_from_asset(asset_handle assetHandle, bool isArray = false)
		{
			const TextureAsset& textureAsset = rLibrary->get_texture_asset(assetHandle);
			return upload_texture_from_asset(textureAsset, isArray);
		}

		ModelUploadData create_model_from_asset(asset_handle assetHandle)
		{
			auto& resources = rRHI->get_resource_handler();
			const ModelAsset& modelAsset = rLibrary->get_model_asset(assetHandle);
			
			// VBO:
			BufferDescriptor bufferDescriptor;
			bufferDescriptor.type = BufferType::VERTEX_BUFFER;
			
			buffer_handle vbo = resources.create_buffer(
				bufferDescriptor, modelAsset.meshData.vertices.data(), 
				{ .size = modelAsset.meshData.vertices.size() }
			);

			// EBO:
			bufferDescriptor.type = BufferType::INDEX_BUFFER;
			buffer_handle ebo = resources.create_buffer(
				bufferDescriptor, modelAsset.meshData.indices.data(), 
				{ .size = modelAsset.meshData.indices.size() }
			);

			// Create textures:
			texture_handle albedoTextureHandle = invalid_handle;
			const auto& albedoTexture = modelAsset.meshMaterial.albedoTexture;
			if(albedoTexture.width > 0 && albedoTexture.height > 0)
				albedoTextureHandle = upload_texture_from_asset(modelAsset.meshMaterial.albedoTexture, false);

			MetallicRoughnessMaterial material;
			{
				material.albedo = albedoTextureHandle;
				material.baseColor = modelAsset.meshMaterial.baseColorFactor;
			}

			return ModelUploadData {
				.modelAssetHandle = assetHandle,
				.vbo = vbo,
				.ebo = ebo,
				.indexCount = modelAsset.meshData.indices.size(),
				.material = material
			};
		}

	private:
		void update_texture_handle(const TextureUploadedEvent& textureUploaded)
		{
			auto view = rScene->get_registry().view<Sprite2DComponent>();
			for (auto [e, sprite] : view.each())
			{
				if (sprite.assetHandle == textureUploaded.assetHandle)
					sprite.textureHandle = textureUploaded.textureHandle;
			}
		}

		void update_texture_array_handle(const TextureUploadedEvent& textureUploaded)
		{
			auto view = rScene->get_registry().view<Tilemap2DComponent>();
			for (auto [e, tilemap] : view.each())
			{
				if (tilemap.assetHandle == textureUploaded.assetHandle)
					tilemap.textureHandle = textureUploaded.textureHandle;
			}
		}

		void update_model_handles(const ModelUploadedEvent& modelUploadedEvent)
		{
			auto view = rScene->get_registry().view<MeshComponent>();
			for (auto [e, mesh] : view.each())
			{
				if (mesh.assetHandle != modelUploadedEvent.data.modelAssetHandle)
					continue;
				
				mesh.vbo = modelUploadedEvent.data.vbo;
				mesh.ebo = modelUploadedEvent.data.ebo;
				mesh.indexCount = modelUploadedEvent.data.indexCount;

				mesh.baseColor = modelUploadedEvent.data.material.baseColor; 
				mesh.albedo = modelUploadedEvent.data.material.albedo; 
			}
		}

	private:
		texture_handle upload_texture_from_asset(const TextureAsset& textureAsset, bool isArray)
		{
			const TextureOptions& options = textureAsset.options;

			TextureDescriptor descriptor;
			{
				descriptor.width = textureAsset.width;
				descriptor.height = textureAsset.height;
				descriptor.layerWidth = options.layerWidth;
				descriptor.layerHeight = options.layerHeight;
				descriptor.depth = options.depth;
				descriptor.mipLevels = options.mipLevels;
				descriptor.samples = options.samples;
				descriptor.arrayLayers = options.arrayLayers;

				descriptor.format = options.format;
				descriptor.type = (isArray) ? TextureType::ARRAY_TEXTURE_2D : TextureType::TEXTURE_2D;

				descriptor.generateMipmaps = options.generateMipmaps;

				descriptor.wrapS = options.wrapS;
				descriptor.wrapT = options.wrapT;
				descriptor.wrapR = options.wrapR;
				descriptor.minFilter = options.minFilter;
				descriptor.magFilter = options.magFilter;

				descriptor.tiling = options.tiling;
			}

			auto& resources = rRHI->get_resource_handler();
			return resources.create_texture(descriptor, textureAsset);
		}

	private:
		NON_OWNING RenderInterface* rRHI = nullptr;
		NON_OWNING AssetLibrary* rLibrary = nullptr;
	};
}