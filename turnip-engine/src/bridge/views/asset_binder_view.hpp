#pragma once
#include "assets/cubemap/cubemap_asset.hpp"
#include "assets/font/font_asset.hpp"
#include "assets/material/material_asset.hpp"
#include "assets/mesh/mesh_asset.hpp"
#include "assets/texture/texture_asset.hpp"
#include "assets/texture/texture_options.hpp"
#include "defines.hpp"
#include "event/asset_events/material_uploaded_event.hpp"
#include "event/asset_events/mesh_uploaded_event.hpp"
#include "event/asset_events/texture_uploaded_event.hpp"
#include "graphics/mesh_vertex.hpp"
#include "graphics/objects/buffer.hpp"
#include "graphics/objects/descriptor.hpp"
#include "graphics/objects/material.hpp"
#include "graphics/objects/pipeline.hpp"
#include "graphics/objects/texture.hpp"
#include "assets/asset.hpp"
#include "event/events.hpp"
#include "graphics/types/pipeline_stage.hpp"
#include "rhi/rhi.hpp"
#include "scene/components.hpp"

#include "scene/components.hpp"
#include "assets/asset_library.hpp"
#include "scene/components/graphics.hpp"
#include "view/view.hpp"
#include <cstring>

namespace tur
{
	class AssetBinderView : public View
	{
	public:
		AssetBinderView(NON_OWNING RenderInterface* rhi, NON_OWNING AssetLibrary* library)
			: rRHI(rhi)
			, rLibrary(library) { }

	public:
		void on_event(Event& event) override
		{
			Subscriber subscriber(event);
			subscriber.subscribe<SceneSwitchedEvent>([this](const SceneSwitchedEvent& event) -> bool {
				rScene = event.currentScene;
				return false;
			});

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

			subscriber.subscribe<MeshUploadedEvent>(
				[&](const MeshUploadedEvent& meshUploadedEvent) -> bool
				{
					handle_mesh_uploaded_event(meshUploadedEvent);
					return false;
				}
			);
			
			subscriber.subscribe<MaterialUploadedEvent>(
				[&](const MaterialUploadedEvent& materialUploadedEvent) -> bool
				{
					handle_material_uploaded_event(materialUploadedEvent);
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

				case AssetType::CUBEMAP:
				{
					texture_handle textureHandle = create_cubemap_texture_from_asset(event.assetHandle);
					TextureUploadedEvent uploadEvent(event.assetHandle, textureHandle, event.type);
					callback(uploadEvent);

				} break;

				case AssetType::MESH:
				{
					auto data = create_mesh_from_asset(event.assetHandle);
					MeshUploadedEvent uploadEvent(data);
					callback(uploadEvent);
				} break;

				case AssetType::FONT:
				{
					texture_handle textureHandle = create_font_texture_from_asset(event.assetHandle);
					TextureUploadedEvent uploadEvent(event.assetHandle, textureHandle, event.type);
					callback(uploadEvent);
				} break;

				case AssetType::AUDIO:
				{
					/* Blank */
				} break;

				case AssetType::MATERIAL:
				{
					material_handle materialHandle = create_material_from_asset(event.assetHandle);
					MaterialUploadedEvent uploadEvent(event.assetHandle, materialHandle);
					callback(uploadEvent);
				} break;

				default:
					return;
			}
		}

		void handle_texture_uploaded_event(const TextureUploadedEvent& textureUploaded)
		{
			if(textureUploaded.textureHandle == invalid_handle)
			{
				TUR_LOG_ERROR("Attempted to upload an invalid texture handle.");
				return;
			}

			switch(textureUploaded.type)
			{
				case AssetType::TEXTURE:
					update_texture_handle(textureUploaded);
					break;

				case AssetType::TEXTURE_ARRAY:
					update_texture_array_handle(textureUploaded);
					break;

				case AssetType::CUBEMAP:
					update_cubemap_handle(textureUploaded);
					break;

				case tur::AssetType::FONT:
					update_font_handle(textureUploaded);
					break;

				default:
					TUR_LOG_ERROR("Attempted to register an invalid asset type as texture.");
			}
		}

		void handle_mesh_uploaded_event(const MeshUploadedEvent& meshUploadedEvent)
		{
			update_mesh_handles(meshUploadedEvent);
		}

		void handle_material_uploaded_event(const MaterialUploadedEvent& materialUploadedEvent)
		{
			update_material_handles(materialUploadedEvent);
		}

	private:
		texture_handle create_texture_from_asset(asset_handle assetHandle, bool isArray = false)
		{
			const TextureAsset& textureAsset = rLibrary->get_texture_asset(assetHandle);
			return upload_texture_from_asset(textureAsset, isArray);
		}

		texture_handle create_cubemap_texture_from_asset(asset_handle assetHandle)
		{
			const CubemapAsset& cubemapAsset = rLibrary->get_cubemap_asset(assetHandle);
			return upload_cubemap_from_asset(cubemapAsset);
		}

		MeshUploadData create_mesh_from_asset(asset_handle assetHandle)
		{
			auto& resources = rRHI->get_resource_handler();
			const MeshAsset& meshAsset = rLibrary->get_mesh_asset(assetHandle);

			// Create textures:
			u64 indexCount = meshAsset.meshData.indices.size();
			switch(meshAsset.meshData.indexType)
			{
				case tur::BufferIndexType::UNSIGNED_BYTE:
					indexCount /= sizeof(byte);
					break;

				case tur::BufferIndexType::UNSIGNED_SHORT:
					indexCount /= sizeof(unsigned short);
					break;

				case tur::BufferIndexType::UNSIGNED_INT:
					indexCount /= sizeof(unsigned int);
					break;
			}

			return MeshUploadData {
				.meshAssetHandle = assetHandle,
				.indexType = meshAsset.meshData.indexType,
				.indexCount = indexCount,
			};
		}

		texture_handle create_font_texture_from_asset(asset_handle assetHandle)
		{
			const FontAsset& fontAsset = rLibrary->get_font_asset(assetHandle);
			return upload_font_from_asset(fontAsset);
		}

		material_handle create_material_from_asset(asset_handle assetHandle)
		{
			const MaterialAsset& material = rLibrary->get_material_asset(assetHandle);
			return upload_material_from_asset(material);
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

		void update_cubemap_handle(const TextureUploadedEvent& cubemapUploaded)
		{
			auto view = rScene->get_registry().view<CubemapComponent>();
			for (auto [e, cubemap] : view.each())
			{
				if (cubemap.assetHandle == cubemapUploaded.assetHandle)
					cubemap.textureHandle = cubemapUploaded.textureHandle;
			}
		}

		void update_font_handle(const TextureUploadedEvent& fontUploaded)
		{
			auto view = rScene->get_registry().view<TextComponent>();
			for (auto [e, text] : view.each())
			{
				if (text.assetHandle == fontUploaded.assetHandle)
					text.textureHandle = fontUploaded.textureHandle;
			}
		}

		void update_mesh_handles(const MeshUploadedEvent& meshUploadedEvent)
		{
			auto view = rScene->get_registry().view<MeshComponent>();
			for (auto entity : view)
			{
				auto& mesh = view.get<MeshComponent>(entity);

				if (mesh.assetHandle != meshUploadedEvent.data.meshAssetHandle)
					continue;

				mesh.indexCount = meshUploadedEvent.data.indexCount;
				mesh.indexType = meshUploadedEvent.data.indexType;
			}
		}

		void update_material_handles(const MaterialUploadedEvent& materialUploadedEvent)
		{
			auto view = rScene->get_registry().view<MaterialComponent>();
			for (auto entity : view)
			{
				auto& material = view.get<MaterialComponent>(entity);

				if (material.assetHandle != materialUploadedEvent.assetHandle)
					continue;

				material.materialHandle = materialUploadedEvent.materialHandle;
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

		texture_handle upload_cubemap_from_asset(const CubemapAsset& cubemapAsset)
		{
			const TextureOptions& options = cubemapAsset.options;

			TextureDescriptor descriptor;
			{
				descriptor.width = cubemapAsset.width;
				descriptor.height = cubemapAsset.height;
				descriptor.layerWidth = options.layerWidth;
				descriptor.layerHeight = options.layerHeight;
				descriptor.depth = options.depth;
				descriptor.mipLevels = options.mipLevels;
				descriptor.samples = options.samples;
				descriptor.arrayLayers = 6;

				descriptor.format = options.format;
				descriptor.type = TextureType::CUBE_MAP;

				descriptor.generateMipmaps = options.generateMipmaps;

				descriptor.wrapS = options.wrapS;
				descriptor.wrapT = options.wrapT;
				descriptor.wrapR = options.wrapR;
				descriptor.minFilter = options.minFilter;
				descriptor.magFilter = options.magFilter;

				descriptor.tiling = options.tiling;
			}

			auto& resources = rRHI->get_resource_handler();

			TextureAsset textureAsset = {};
			{
				textureAsset.data = cubemapAsset.data;
				textureAsset.width = cubemapAsset.width;
				textureAsset.height = cubemapAsset.height;
				textureAsset.channels = cubemapAsset.channels;
				textureAsset.options = cubemapAsset.options;
			}

			return resources.create_texture(descriptor, textureAsset);
		}

		texture_handle upload_font_from_asset(const FontAsset& fontAsset)
		{
			auto& resources = rRHI->get_resource_handler();
			const TextureOptions& options = fontAsset.options;
			
			TextureDescriptor descriptor;
			{
				descriptor.width = fontAsset.maxWidth * fontAsset.characters.size();
				descriptor.height = fontAsset.maxHeight;
				descriptor.layerWidth = fontAsset.maxWidth;
				descriptor.layerHeight = fontAsset.maxHeight;
				descriptor.depth = options.depth;
				descriptor.mipLevels = options.mipLevels;
				descriptor.samples = options.samples;
				descriptor.arrayLayers = fontAsset.characters.size();

				descriptor.format = TextureFormat::R8_UNORM;
				descriptor.type = TextureType::ARRAY_TEXTURE_2D;

				descriptor.generateMipmaps = options.generateMipmaps;

				descriptor.wrapS = options.wrapS;
				descriptor.wrapT = options.wrapT;
				descriptor.wrapR = options.wrapR;
				descriptor.minFilter = options.minFilter;
				descriptor.magFilter = options.magFilter;

				descriptor.tiling = options.tiling;
			}

			texture_handle texture = resources.create_empty_texture(descriptor);

			for(const auto& [id, character] : fontAsset.characters)
			{
				if(!character.buffer.size())
					continue;

				TextureAsset asset;
				{
					asset.options.dataFormat = TextureDataFormat::RED;
					asset.width = fontAsset.maxWidth * fontAsset.characters.size();
					asset.options.layerWidth = character.size.x; 
					asset.options.layerHeight = character.size.y;
					asset.data = character.buffer;
				}

				resources.update_array_texture_layer(texture, asset, { 0.0f, 0.0f }, character.layer);
			}
		
			return texture;
		}

		material_handle upload_material_from_asset(const MaterialAsset& materialAsset)
		{
			auto& resources = rRHI->get_resource_handler();

			// Shaders:
			shader_handle vertexShader = resources.create_shader
			({
				materialAsset.vertexShaderFilepath, ShaderType::VERTEX
			});
			shader_handle fragmentShader = resources.create_shader
			({
				materialAsset.fragmentShaderFilepath, ShaderType::FRAGMENT
			});
			// TODO: add rest of shaders.
			
			/* TODO: SHOULD ALL BE THE SAME? The vertex shader currently only supports one vertex input stage variation */
			auto VertexBindings = std::vector<BindingDescriptor>
			({
				{
					.binding = 0,
					.stride = sizeof(MeshVertex),
					.inputRate = InputRate::VERTEX
				}
			});

			auto Attributes = std::vector<Attribute>
			({
				{
					.binding = 0,
					.location = 0,
					.offset = offsetof(MeshVertex, position),
					.format = AttributeFormat::R32G32B32_SFLOAT
				},
				{
					.binding = 0,
					.location = 1,
					.offset = offsetof(MeshVertex, uvx),
					.format = AttributeFormat::R32_SFLOAT
				},
				{
					.binding = 0,
					.location = 2,
					.offset = offsetof(MeshVertex, normals),
					.format = AttributeFormat::R32G32B32_SFLOAT
				},
				{
					.binding = 0,
					.location = 3,
					.offset = offsetof(MeshVertex, uvy),
					.format = AttributeFormat::R32_SFLOAT
				},
			});

			constexpr auto DynamicStates = std::to_array<DynamicState>({
				DynamicState::VIEWPORT,
				DynamicState::SCISSOR
			});
			
			constexpr auto Viewports = std::to_array<Viewport>({Viewport{}});
			constexpr auto Scissors = std::to_array<Extent2D>({Extent2D{}});

			// Pipeline layout:
			// TODO: bindless textures
			DescriptorSetLayoutDescriptor setDescriptor = {};
			{
				setDescriptor.entries = 
				{
					DescriptorSetLayoutEntry
					{
						.binding = 0,
						.amount = 1,
						.type = DescriptorType::STORAGE_BUFFER,
						.stage = PipelineStage::VERTEX_STAGE,
					},
					DescriptorSetLayoutEntry
					{
						.binding = 1,
						.amount = 1,
						.type = DescriptorType::STORAGE_BUFFER,
						.stage = PipelineStage::VERTEX_STAGE,
					},
				};
			}
			auto setLayout = resources.create_descriptor_set_layout(setDescriptor);

			// Pipeline:
			PipelineDescriptor pipelineDescriptor = {
				.vertexInputStage = 
				{
					.vertexBindings = VertexBindings,
					.attributes = Attributes
				},
				.inputAssemblyStage = materialAsset.inputAssemblyStage,
				.rasterizerStage = materialAsset.rasterizerStage,
				.setLayout = setLayout,
				.viewports = Viewports,
				.scissors = Scissors,
				.dynamicStates = DynamicStates,
				.vertexShader = vertexShader,
				.fragmentShader = fragmentShader,
			};

			auto pipeline = resources.create_graphics_pipeline(pipelineDescriptor);
			return resources.create_material(pipeline, setLayout);
		}

	private:
		NON_OWNING RenderInterface* rRHI = nullptr;
		NON_OWNING AssetLibrary* rLibrary = nullptr;
		NON_OWNING Scene* rScene = nullptr;
	};
}
