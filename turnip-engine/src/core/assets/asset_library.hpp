#pragma once
#include "worker/worker_pool.hpp"
#include <filesystem>

namespace tur::asset
{
	class AssetLibrary
	{
	public:
		void initialize(NON_OWNING WorkerPool* workerPool);

	public:
		void load_texture_async(const std::filesystem::path& filepath);

	private:
		NON_OWNING WorkerPool* workerPool = nullptr;
	};
}