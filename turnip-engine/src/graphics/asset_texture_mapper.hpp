#pragma once
#include "assets/asset_library.hpp"
#include "event/events.hpp"
#include "rhi/rhi.hpp"

namespace tur
{
	class AssetTextureMapper
	{
	public:
		void initialize(RenderInterface* rhi, AssetLibrary* assetLibrary)
		{
			rRHI = rhi;
			rAssetLibrary = assetLibrary;
		}

		void on_event(Event& event)
		{
			auto& resources = rRHI->get_resource_handler();

			Subscriber subscriber(event);
			subscriber.subscribe<AssetLoadedEvent>(
				[&](const AssetLoadedEvent& event) -> bool
				{
					TextureAsset textureAsset = rAssetLibrary->get_texture(event.assetHandle);

					TextureDescriptor descriptor;
					{
						descriptor.width = textureAsset.width;
						descriptor.height = textureAsset.height;
						descriptor.depth = 1;
						descriptor.mipLevels = 1;
						descriptor.samples = 1;
						descriptor.type = TextureType::TEXTURE_2D;
					}

					texture_handle textureHandle = resources.create_texture(descriptor, textureAsset);
					mTextureMap[event.assetHandle] = textureHandle;
					return false;
				}
			);
		}

	private:
		NON_OWNING RenderInterface* rRHI = nullptr;
		NON_OWNING AssetLibrary* rAssetLibrary = nullptr;

	private:
		std::unordered_map<asset_handle, texture_handle> mTextureMap;
	};
}