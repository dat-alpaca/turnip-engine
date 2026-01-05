#include "asset_library.hpp"
#include <miniaudio.h>
#include <optional>
#include <stb_image.h>

#include "assets/model/model_asset.hpp"
#include "graphics/constants.hpp"
#include "assets/asset.hpp"

#include "graphics/objects/texture.hpp"
#include "logging/logging.hpp"
#include "texture/texture_loader.hpp"
#include "model/model_loader.hpp"
#include "audio/audio_loader.hpp"
#include "utils/uuid/uuid.hpp"

namespace tur
{
	void AssetLibrary::initialize(WorkerPool* workerPool, AudioHandler* audioHandler)
	{
		TUR_ASSERT(workerPool, "Invalid WorkerPool handle passed");
		TUR_ASSERT(audioHandler, "Invalid AudioHandler handle passed");

		rAudioHandler = audioHandler;
		rWorkerPool = workerPool;

		create_default_texture();
	}
	void AssetLibrary::shutdown()
	{
		mTextureAssets.clear();
		mAudioAssets.clear();
	}

	void AssetLibrary::set_event_callback(const EventCallback& eventCallback)
	{
		mEventCallback = eventCallback;
	}

	asset_handle AssetLibrary::load_texture_async(const std::filesystem::path& filepath, const TextureOptions& options)
	{
		if (mFilepathCache.find(filepath) != mFilepathCache.end())
			return mFilepathCache[filepath];

		TextureAsset loadingAsset = {};
		loadingAsset.options = options;
		loadingAsset.metadata.uuid = UUID();
		loadingAsset.metadata.filepath = filepath;
		loadingAsset.state = AssetState::LOADING;

		asset_handle assetHandle = mTextureAssets.add(loadingAsset);
		mFilepathCache[filepath] = assetHandle;

		rWorkerPool->submit<texture_asset_opt>(
			[filepath, options]()
			{
				if (!options.floatTexture)
					return load_texture_task(filepath);
				else
					return load_float_texture_task(filepath);
			},
			[&, filepath, assetHandle](const texture_asset_opt& textureAsset)
			{
				if (!textureAsset.has_value())
					return TUR_LOG_ERROR("Failed to load texture asset into the asset library.");

				TextureAsset& loadedTextureAsset = mTextureAssets.get(assetHandle);
				{
					loadedTextureAsset.data = textureAsset->data;
					loadedTextureAsset.width = textureAsset->width;
					loadedTextureAsset.height = textureAsset->height;
					loadedTextureAsset.channels = textureAsset->channels;
					loadedTextureAsset.state = AssetState::READY;
				}

				AssetType type = AssetType::TEXTURE;
				if(loadedTextureAsset.options.arrayLayers > 1)
					loadedTextureAsset.options.isTextureArray = true;

				if(loadedTextureAsset.options.isTextureArray)
					type = AssetType::TEXTURE_ARRAY;

				AssetLoadedEvent assetLoadedEvent(assetHandle, type);
				mEventCallback(assetLoadedEvent);
			}
		);

		return assetHandle;
	}

	asset_handle AssetLibrary::load_audio_async(const std::filesystem::path& filepath)
	{
		if (mFilepathCache.find(filepath) != mFilepathCache.end())
			return mFilepathCache[filepath];

		auto& engine = rAudioHandler->get_engine();

		asset_handle assetHandle = mAudioAssets.add({});
		AudioAsset& audioAsset = mAudioAssets.get(assetHandle);

		audioAsset.metadata.filepath = filepath;
		audioAsset.state = AssetState::READY;

		auto audioResult = load_audio_task(filepath, &audioAsset.sound, rAudioHandler->mEngine);

		if (audioResult != MA_SUCCESS)
		{
			TUR_LOG_ERROR("Failed to load audio: {}", filepath.string());
			mAudioAssets.remove(assetHandle);
			return invalid_handle;
		}

		mFilepathCache[filepath] = assetHandle;

		AssetLoadedEvent assetLoadedEvent(assetHandle, AssetType::AUDIO);
		mEventCallback(assetLoadedEvent);

		return assetHandle;
	}

	struct InternalData
	{
		tinygltf::Model model;
	};

	asset_handle AssetLibrary::load_model_async(const std::filesystem::path& filepath)
	{
		if (mFilepathCache.find(filepath) != mFilepathCache.end())
			return mFilepathCache[filepath];

		ModelAsset loadingAsset = {};
		loadingAsset.metadata.filepath = filepath;
		loadingAsset.metadata.uuid = UUID();
		loadingAsset.state = AssetState::LOADING;

		auto parentPath = filepath.parent_path();

		asset_handle assetHandle = mModelAssets.add(loadingAsset);
		mFilepathCache[filepath] = assetHandle;

		rWorkerPool->submit<std::optional<InternalData>>([&, filepath, parentPath]() -> std::optional<InternalData> {
			auto model = load_model_task(filepath);
			if(!model.has_value())
				return std::nullopt;

			InternalData data;
			data.model = model.value();
			return data;
		},
		[&, assetHandle](const std::optional<InternalData>& data) {
			if (!data.has_value())
				return TUR_LOG_ERROR("Failed to load texture asset into the asset library.");

			ModelAsset& loadedModelAsset = mModelAssets.get(assetHandle);
			loadedModelAsset.model = data.value().model;
			loadedModelAsset.state = AssetState::READY;

			AssetLoadedEvent assetLoadedEvent(assetHandle, AssetType::MODEL);
			mEventCallback(assetLoadedEvent);
		});

		return assetHandle;
	}
}

namespace tur
{
	void AssetLibrary::create_default_texture()
	{
		TextureAsset asset;
		{
			asset.metadata.filepath = "internal/default_texture";
			asset.metadata.uuid = UUID();

			asset.width = DefaultTexture::Width;
			asset.height = DefaultTexture::Height;
			asset.channels = DefaultTexture::Channels;

			u32 size = asset.width * asset.height * asset.channels;
			asset.data = std::vector<byte>(DefaultTexture::Data, DefaultTexture::Data + size);
		}

		DefaultTextureHandle = mTextureAssets.add(asset);
		mFilepathCache["internal/default_texture"] = DefaultTextureHandle;
	}
}