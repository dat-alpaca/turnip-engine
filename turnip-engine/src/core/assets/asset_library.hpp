#pragma once
#include "worker/worker_pool.hpp"
#include <filesystem>

namespace tur::asset
{
	class AssetLibrary
	{
	public:
		void initialize(WorkerPool* workerPool, TextureAssetBinder* binder);
		void set_event_callback(EventCallback& eventCallback);

	public:
		void load_texture_async(const std::filesystem::path& filepath);

	private:
		NON_OWNING WorkerPool* workerPool = nullptr;
	};
}