#include "turnip_engine.hpp"
#include "assets/asset_library.hpp"
#include "core/config/config_data.hpp"
#include "defines.hpp"

#include "client/scene_serialization.hpp"
#include "graphics/constants.hpp"
#include "graphics/objects/texture.hpp"
#include "graphics/types/queue_operations.hpp"

namespace tur::engine
{
	static void startup(ViewHandler& viewHandler)
	{
		for (const auto& view : viewHandler.get_views())
			view->on_engine_startup();
	}
	static void shutdown(ViewHandler& viewHandler)
	{
		for (const auto& view : viewHandler.get_views())
			view->on_engine_shutdown();
	}

	static void on_fixed_update(ViewHandler& viewHandler)
	{
		for (const auto& view : viewHandler.get_views())
			view->on_fixed_update();
	}
	static void on_update(ViewHandler& viewHandler, const Time& time)
	{
		for (const auto& view : viewHandler.get_views())
			view->on_update(time);

		for (const auto& view : viewHandler.get_views())
			view->on_post_update(time);
	}
	static void on_render(RenderInterface& renderInterface, ViewHandler& viewHandler)
	{
		if (!renderInterface.begin_frame())
			return;

		CommandBuffer commandBuffer = renderInterface.create_command_buffer();
		commandBuffer.reset(renderInterface.get_current_command_buffer());

		for (const auto& view : viewHandler.get_views())
			view->on_render();

		renderInterface.end_frame();
		renderInterface.submit(renderInterface.get_queue(QueueOperation::GRAPHICS));
		renderInterface.present(renderInterface.get_queue(QueueOperation::PRESENT));
	}
	static void on_render_gui(ViewHandler& viewHandler)
	{
		for (const auto& view : viewHandler.get_views())
			view->on_render_gui();
	}

	static void on_event(Event& event, TurnipEngine& engine)
	{
		engine.get_render_interface().on_event(event);
		engine.get_asset_library().on_event(event);

		for (const auto& view : engine.get_view_handler().get_views())
			view->on_event(event);
	}
}

namespace tur
{
	TurnipEngine::TurnipEngine(const std::filesystem::path& configFilepath)
	{
		using namespace tur::platform;

		// Logging:
		initialize_logging_system();

		// Config:
		initialize_config_data(configFilepath);
		auto configDataResult = json_parse_file<ConfigData>(configFilepath);
		{
			if (!configDataResult.has_value())
			{
				TUR_LOG_CRITICAL(
					"Failed to parse the configuration from the specified filepath: {}", configFilepath.string()
				);
				return;
			}
		}
		mConfigData = configDataResult.value();

		// Event Callback:
		mMainEventCallback = [&](Event& event) { engine::on_event(event, *this); };

		// Windowing:
		mWindow = WindowingSystem::initialize(mConfigData.windowingCapabilities, mConfigData.windowProperties);
		mWindow->set_event_callback(mMainEventCallback);

		// Render Interface:
		mRenderInterface.initialize(mConfigData, *mWindow);

		// Script:
		mScriptHandler.initialize(mWindow.get());
		mScriptHandler.initialize_input();
		mScriptHandler.initialize_audio(&mAudioHandler);

		// Worker Pool:
		mWorkerPool.initialize(mConfigData.workerConfig);

		// Audio:
		mAudioHandler.initialize(&mAssetLibrary);

		// Asset Library:
		mAssetLibrary.initialize(&mWorkerPool, &mAudioHandler);
		mAssetLibrary.set_event_callback(mMainEventCallback);

		{
			// Default texture:	
			const auto& asset = mAssetLibrary.get_texture_asset(AssetLibrary::DefaultTextureHandle);
			TextureDescriptor descriptor;
			{
				descriptor.width = DefaultTexture::Width;
				descriptor.height = DefaultTexture::Height;
				descriptor.format = DefaultTexture::Format;
				descriptor.tiling = TextureTiling::RAW;
			}

			mRenderInterface.DefaultTextureHandle = mRenderInterface.get_resource_handler().create_texture(descriptor, asset);
			if(mRenderInterface.DefaultTextureHandle == invalid_handle)
				TUR_LOG_CRITICAL("Failed to create a default texture handle");
		}

		// Physics:
		mPhysicsHandler.initialize(mConfigData.physicsConfig);
	}

	void TurnipEngine::start()
	{
		engine::startup(mViewHandler);

		while (!mShutdownRequested && mWindow->is_open())
		{
			mWindow->poll_events();
			mWorkerPool.poll_tasks();

			mTime.cycle(mWindow->get_time());
			mFixedUpdateTimer += mTime.get_delta_time();

			if(mFixedUpdateTimer >= mConfigData.physicsConfig.fixedTimeStep)
			{
				engine::on_fixed_update(mViewHandler);
				mFixedUpdateTimer = 0.0f;
			}
			
			engine::on_update(mViewHandler, mTime);

			engine::on_render(mRenderInterface, mViewHandler);

			engine::on_render_gui(mViewHandler);
		}

		engine::shutdown(mViewHandler);

		mRenderInterface.shutdown();
		mAssetLibrary.shutdown();
		mAudioHandler.shutdown();

		mWindow->destroy();
		platform::WindowingSystem::shutdown();
	}
	void TurnipEngine::shutdown()
	{
		mShutdownRequested = true;
	}

	view_handle TurnipEngine::add_view(tur_unique<View> view)
	{
		view->set_engine(this);
		view->set_event_callback(mMainEventCallback);
		view_handle handle = mViewHandler.add(std::move(view));

		return handle;
	}
	void TurnipEngine::remove_view(view_handle viewHandle)
	{
		mViewHandler.remove(viewHandle);
	}

	void TurnipEngine::load_scene(const std::filesystem::path& filepath, const Project& project, Scene* scene)
	{
		ScenePreDeserializationEvent preDeserialization(scene);
		mMainEventCallback(preDeserialization);

		deserialize_scene(scene, project, filepath);

		SceneDeserializedEvent deserializeEvent(scene, project);
		mMainEventCallback(deserializeEvent);
	}
}