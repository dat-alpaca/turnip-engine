#pragma once
#include "event/events.hpp"
#include "event_consumer.hpp"
#include "event_emitter.hpp"

#include "assets/asset_library.hpp"
#include "rhi/rhi.hpp"

namespace tur
{
	class TextureAssetBinderAgent : public IEventConsumer, public IEventEmitter
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
				[&](const AssetLoadedEvent& event) -> bool
				{
					if (event.type != AssetType::TEXTURE && event.type != AssetType::TEXTURE_ARRAY)
						return false;

					texture_handle textureHandle = upload_texture(event.assetHandle);

					TextureUploadedEvent uploadEvent(event.assetHandle, textureHandle, event.type);
					callback(uploadEvent);

					return false;
				}
			);
		}

	private:
		texture_handle upload_texture(asset_handle assetHandle)
		{
			auto& resources = rRHI->get_resource_handler();
			const TextureAsset& textureAsset = rLibrary->get_texture_asset(assetHandle);
			const TextureOptions& options = textureAsset.options;

			TextureDescriptor descriptor;
			{
				descriptor.width = textureAsset.width;
				descriptor.height = textureAsset.height;
				descriptor.depth = options.depth;
				descriptor.mipLevels = options.mipLevels;
				descriptor.samples = options.samples;

				descriptor.format = options.format;
				descriptor.type = options.type;

				descriptor.generateMipmaps = options.generateMipmaps;

				descriptor.wrapS = options.wrapS;
				descriptor.wrapT = options.wrapT;
				descriptor.wrapR = options.wrapR;
				descriptor.minFilter = options.minFilter;
				descriptor.magFilter = options.magFilter;

				descriptor.tiling = options.tiling;
			}

			texture_handle textureHandle = resources.create_texture(descriptor, textureAsset);
			return textureHandle;
		}

	private:
		NON_OWNING RenderInterface* rRHI = nullptr;
		NON_OWNING AssetLibrary* rLibrary = nullptr;
	};
}