#include "turnip_engine.hpp"
#include "GLFW/glfw3.h"
#include "assets/asset_library.hpp"
#include "common.hpp"
#include "core/config/config_data.hpp"
#include "graphics/constants.hpp"
#include "graphics/objects/texture.hpp"
#include "graphics/types/queue_operations.hpp"
#include "logging/logging.hpp"
#include "vulkan/objects/vulkan_command_buffer.hpp"

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

	static void on_update(ViewHandler& viewHandler, const Time& time)
	{
		for (const auto& view : viewHandler.get_views())
			view->on_update(time);
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
		ConfigData configData = configDataResult.value();

		// Event Callback:
		mMainEventCallback = [&](Event& event) { engine::on_event(event, *this); };

		// Windowing:
		mWindow = WindowingSystem::initialize(configData.windowingCapabilities, configData.windowProperties);
		mWindow->set_event_callback(mMainEventCallback);

		// Render Interface:
		mRenderInterface.initialize(configData, *mWindow);

		// Script:
		mScriptHandler.initialize();
		mScriptHandler.initialize_input(*mWindow);
		mScriptHandler.initialize_audio(&mAudioHandler);

		// Worker Pool:
		mWorkerPool.initialize(configData.workerConfig);

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
		}

		// Physics:
		mPhysicsHandler.initialize(configData.physicsConfig);
	}

	void TurnipEngine::start()
	{
		engine::startup(mViewHandler);

		while (!mShutdownRequested && mWindow->is_open())
		{
			mTime.cycle(mWindow->get_time());
			mTitleAccumulatedTime += mTime.get_delta_time();

			if(mTitleAccumulatedTime >= 1.f)
			{
				mTitleAccumulatedTime = 0.0f;
				glfwSetWindowTitle(mWindow->get_window(), std::to_string(mTime.get_fps()).c_str());
			}

			mWindow->poll_events();
			mWorkerPool.poll_tasks();

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

	void TurnipEngine::add_view(tur_unique<View> view)
	{
		view->set_engine(this);
		mViewHandler.add(std::move(view));
	}
	void TurnipEngine::remove_view(view_handle viewHandle)
	{
		mViewHandler.remove(viewHandle);
	}
}