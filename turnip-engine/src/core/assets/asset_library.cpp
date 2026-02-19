#include "asset_library.hpp"
#include <filesystem>
#include <miniaudio.h>
#include <optional>
#include <stb_image.h>

#include "assets/cubemap/cubemap_asset.hpp"
#include "assets/mesh/mesh_asset.hpp"
#include "assets/asset.hpp"

#include "defines.hpp"
#include "event/scene_events/scene_switched_event.hpp"
#include "event/subscriber.hpp"
#include "graphics/constants.hpp"

#include "assets/font/font_loader.hpp"
#include "graphics/objects/texture.hpp"
#include "logging/logging.hpp"
#include "texture/texture_loader.hpp"
#include "mesh/mesh_loader.hpp"
#include "audio/audio_loader.hpp"
#include "utils/uuid/uuid.hpp"

namespace tur
{
	void AssetLibrary::initialize(WorkerPool* workerPool, AudioHandler* audioHandler, FontHandler* fontHandler)
	{
		TUR_ASSERT(workerPool, "Invalid WorkerPool handle passed");
		TUR_ASSERT(audioHandler, "Invalid AudioHandler handle passed");
		TUR_ASSERT(fontHandler, "Invalid FontHandler handle passed");

		rAudioHandler = audioHandler;
		rWorkerPool = workerPool;
		rFontHandler = fontHandler;

		create_default_texture();
	}
	void AssetLibrary::on_event(Event& event)
	{
		Subscriber subscriber(event);
		subscriber.subscribe<SceneSwitchedEvent>([&](const SceneSwitchedEvent& event){

			if(mIsFirstSceneSwap)
			{
				mIsFirstSceneSwap = false;
				return false;
			}

			for(auto textureHandle : mTextureRemoveOnSceneChange)
				mTextureAssets.remove(textureHandle);

			for(auto audioHandle : mAudioRemoveOnSceneChange)
				mAudioAssets.remove(audioHandle);

			for(auto meshHandle : mMeshRemoveOnSceneChange)
				mMeshAssets.remove(meshHandle);

			for(auto cubemapHandle : mCubemapRemoveOnSceneChange)
				mCubemapAssets.remove(cubemapHandle);

			return false;
		});
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

	asset_handle AssetLibrary::load_texture_async(const std::filesystem::path& filepath, const AssetMetadata& metadata, const TextureOptions& options)
	{
		if (mFilepathCache.find(filepath) != mFilepathCache.end())
			return mFilepathCache[filepath];

		TextureAsset loadingAsset = {};
		loadingAsset.options = options;
		loadingAsset.metadata = metadata;
		loadingAsset.state = AssetState::LOADING;

		asset_handle assetHandle = mTextureAssets.add(loadingAsset);
		mFilepathCache[filepath] = assetHandle;
		mUUIDCache[metadata.uuid] = assetHandle;

		if (metadata.lifetime == AssetLifetime::SCENE_BOUND)
			mTextureRemoveOnSceneChange.push_back(assetHandle);

		rWorkerPool->submit<texture_asset_opt>(
			[filepath, options]()
			{
				if (!options.floatTexture)
					return load_texture_task(filepath);
				else
					return load_float_texture_task(filepath);
			},
			[&, assetHandle](const texture_asset_opt& textureAsset)
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

	asset_handle AssetLibrary::load_audio_async(const std::filesystem::path& filepath, const AssetMetadata& metadata)
	{
		if (mFilepathCache.find(filepath) != mFilepathCache.end())
			return mFilepathCache[filepath];

		auto& engine = rAudioHandler->get_engine();

		asset_handle assetHandle = mAudioAssets.add({});
		AudioAsset& audioAsset = mAudioAssets.get(assetHandle);

		audioAsset.metadata = metadata;
		audioAsset.state = AssetState::READY;

		if(metadata.lifetime == AssetLifetime::SCENE_BOUND)
			mAudioRemoveOnSceneChange.push_back(assetHandle);

		auto audioResult = load_audio_task(filepath, &audioAsset.sound, rAudioHandler->mEngine);

		if (audioResult != MA_SUCCESS)
		{
			TUR_LOG_ERROR("Failed to load audio: {}", filepath.string());
			mAudioAssets.remove(assetHandle);
			return invalid_handle;
		}

		mFilepathCache[filepath] = assetHandle;
		mUUIDCache[audioAsset.metadata.uuid] = assetHandle;

		AssetLoadedEvent assetLoadedEvent(assetHandle, AssetType::AUDIO);
		mEventCallback(assetLoadedEvent);

		return assetHandle;
	}

	asset_handle AssetLibrary::load_mesh_async(const std::filesystem::path& filepath, const AssetMetadata& metadata)
	{
		if (mFilepathCache.find(filepath) != mFilepathCache.end())
			return mFilepathCache[filepath];

		MeshAsset loadingAsset = {};
		loadingAsset.metadata = metadata;
		loadingAsset.state = AssetState::LOADING;

		auto parentPath = filepath.parent_path();
		asset_handle assetHandle = mMeshAssets.add(loadingAsset);
		mFilepathCache[filepath] = assetHandle;
		mUUIDCache[metadata.uuid] = assetHandle;

		if(metadata.lifetime == AssetLifetime::SCENE_BOUND)
			mMeshRemoveOnSceneChange.push_back(assetHandle);

		rWorkerPool->submit<std::optional<MeshAsset>>([&, filepath, parentPath]() -> std::optional<MeshAsset> {
			auto modelResult = load_model_task(filepath);
			if(!modelResult.has_value())
				return std::nullopt;

			auto model = modelResult.value();
			
			// TODO: extract mesh data from all meshes

			MeshAsset asset;
			asset.meshData = extract_mesh_data(model);
			asset.meshMaterial = extract_texture_data(model);

			return asset;
		},
		[&, assetHandle](const std::optional<MeshAsset>& asset) {
			
			if (!asset.has_value())
				return TUR_LOG_ERROR("Failed to load texture asset into the asset library.");
			
			auto mesh = asset.value();

			MeshAsset& loadedMeshAsset = mMeshAssets.get(assetHandle);
			loadedMeshAsset.meshMaterial = mesh.meshMaterial;
			loadedMeshAsset.meshData = mesh.meshData;
			loadedMeshAsset.state = AssetState::READY;

			AssetLoadedEvent assetLoadedEvent(assetHandle, AssetType::MESH);
			mEventCallback(assetLoadedEvent);
		});

		return assetHandle;
	}

	asset_handle AssetLibrary::load_cubemap_async(const std::array<std::filesystem::path, 6>& filepaths, const CubemapAsset& asset)
	{
		if (mFilepathCache.find(asset.metadata.filepath) != mFilepathCache.end())
			return mFilepathCache[asset.metadata.filepath];

		CubemapAsset loadingAsset = {};
		loadingAsset.options = asset.options;
		loadingAsset.metadata = asset.metadata;
		loadingAsset.state = AssetState::LOADING;

		asset_handle assetHandle = mCubemapAssets.add(loadingAsset);
		mFilepathCache[asset.metadata.filepath] = assetHandle;
		mUUIDCache[asset.metadata.uuid] = assetHandle;

		if (asset.metadata.lifetime == AssetLifetime::SCENE_BOUND)
			mCubemapRemoveOnSceneChange.push_back(assetHandle);

		rWorkerPool->submit<std::optional<CubemapAsset>>(
			[filepaths]()
			{
				return load_cubemap_task(filepaths);
			},
			[this, assetHandle](const std::optional<CubemapAsset>& cubemapAsset)
			{
				if (!cubemapAsset.has_value())
				{
					TUR_LOG_ERROR("Failed to load cubemap asset into the asset library.");
					return;
				}
		
				CubemapAsset& loadedCubemapAsset = mCubemapAssets.get(assetHandle);
				{
					loadedCubemapAsset.data = cubemapAsset->data;
					loadedCubemapAsset.width = cubemapAsset->width;
					loadedCubemapAsset.height = cubemapAsset->height;
					loadedCubemapAsset.channels = cubemapAsset->channels;
					loadedCubemapAsset.state = AssetState::READY;
				}

				AssetLoadedEvent assetLoadedEvent(assetHandle, AssetType::CUBEMAP);
				mEventCallback(assetLoadedEvent);
			}
		);

		return assetHandle;
	}

	asset_handle AssetLibrary::load_font_async(const std::filesystem::path& filepath, const AssetMetadata& metadata, const TextureOptions& options, u32 height, bool loadSDF)
	{
		if (mFilepathCache.find(filepath) != mFilepathCache.end())
			return mFilepathCache[filepath];

		auto& fonts = rFontHandler->get_font_library();

		asset_handle assetHandle = mFontAssets.add({});
		FontAsset& fontAsset = mFontAssets.get(assetHandle);
		fontAsset.metadata = metadata;
		fontAsset.options = options;
		fontAsset.isSDF = loadSDF;

		if(metadata.lifetime == AssetLifetime::SCENE_BOUND)
			mFontRemoveOnSceneChange.push_back(assetHandle);

		mFilepathCache[filepath] = assetHandle;
		mUUIDCache[fontAsset.metadata.uuid] = assetHandle;

		rWorkerPool->submit<std::optional<FontAsset>>(
			[fonts, filepath, metadata, options, height, loadSDF]()
			{
				return load_font_task(filepath, fonts, metadata, options, height, loadSDF);
			},
			[this, assetHandle](const std::optional<FontAsset>& fontAsset)
			{
				if (!fontAsset.has_value())
				{
					TUR_LOG_ERROR("Failed to load font asset into the asset library.");
					return;
				}
		
				FontAsset& loadedFontAsset = mFontAssets.get(assetHandle);
				{
					loadedFontAsset.characters = fontAsset->characters;
					loadedFontAsset.face = fontAsset->face;
					loadedFontAsset.state = AssetState::READY;
					loadedFontAsset.height = fontAsset->height;
					loadedFontAsset.maxHeight = fontAsset->maxHeight;
					loadedFontAsset.maxWidth = fontAsset->maxWidth;
				}

				AssetLoadedEvent assetLoadedEvent(assetHandle, AssetType::FONT);
				mEventCallback(assetLoadedEvent);
			}
		);

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