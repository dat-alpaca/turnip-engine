#include "turnip_engine.hpp"
#include "common.hpp"
#include "core/config/config_data.hpp"
#include "logging/logging.hpp"

namespace tur::engine
{
	static void startup(ViewSystem& viewSystem)
	{
		for (const auto& view : viewSystem.get_views())
			view->on_engine_startup();
	}
	static void shutdown(ViewSystem& viewSystem)
	{
		for (const auto& view : viewSystem.get_views())
			view->on_engine_shutdown();
	}

	static void on_update(ViewSystem& viewSystem)
	{
		for (const auto& view : viewSystem.get_views())
			view->on_update();
	}
	static void on_render(RenderInterface& renderInterface, ViewSystem& viewSystem)
	{
		for (const auto& view : viewSystem.get_views())
			view->on_render();
	}
	static void on_render_gui(ViewSystem& viewSystem)
	{
		for (const auto& view : viewSystem.get_views())
			view->on_render_gui();
	}

	static void on_event(Event& event, TurnipEngine& engine)
	{
		// Systems:
		engine.get_render_interface().on_event(event);

		for (const auto& view : engine.get_view_system().get_views())
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

		// Windowing:
		mWindow = WindowingSystem::initialize(configData.windowingCapabilities, configData.windowProperties);
		mWindow->set_event_callback([&](Event& event) { engine::on_event(event, *this); });

		// Render Interface:
		mRenderInterface.initialize(configData, *mWindow);

		// Scene Management:
		mSceneHolder.set_event_callback([&](Event& event) { engine::on_event(event, *this); });

		// Script:
		ScriptSystem::initialize_core();
		ScriptSystem::initialize_input(*mWindow);

		// Worker Pool:
		mWorkerPool.initialize(configData.workerConfig);

		// Asset Library:
		mAssetLibrary.initialize(&mWorkerPool, &mTextureAssetBinder);
		mAssetLibrary.set_event_callback([&](Event& event) { engine::on_event(event, *this); });

		// Binders:
		mTextureAssetBinder.initialize(&mRenderInterface, &mAssetLibrary);
	}

	void TurnipEngine::start()
	{
		engine::startup(mViewSystem);

		while (!mShutdownRequested && mWindow->is_open())
		{
			mWindow->poll_events();
			mWorkerPool.poll_tasks();

			engine::on_update(mViewSystem);

			engine::on_render(mRenderInterface, mViewSystem);

			engine::on_render_gui(mViewSystem);
		}

		engine::shutdown(mViewSystem);

		mRenderInterface.shutdown();

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
		mViewSystem.add(std::move(view));
	}
	void TurnipEngine::remove_view(view_handle viewHandle)
	{
		mViewSystem.remove(viewHandle);
	}
}