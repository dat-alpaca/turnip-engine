#include "worker_pool.hpp"

namespace tur
{
	WorkerPool::~WorkerPool()
	{
		{
			std::unique_lock<std::mutex> lock(mQueueMutex);
			mStopExecution = true;
		}

		mConditionalVar.notify_all();

		for (auto& thread : mThreads)
			thread.join();
	}

	void WorkerPool::initialize(const WorkerConfig& workerConfig)
	{
		u64 threadAmount =
			(workerConfig.threadAmount == 0) ? std::thread::hardware_concurrency() : workerConfig.threadAmount;

		for (size_t i = 0; i < threadAmount; ++i)
			mThreads.emplace_back(std::bind(&WorkerPool::worker_function, this));
	}

	void WorkerPool::poll_tasks()
	{
		std::unique_lock<std::mutex> lock(mQueueMutex);

		for (const auto& task : mCallbacks)
			task();

		mCallbacks.clear();
	}

	void WorkerPool::worker_function()
	{
		while (true)
		{
			task_t task;

			{
				std::unique_lock<std::mutex> lock(mQueueMutex);

				mConditionalVar.wait(lock, [this] { return !mTasks.empty() || mStopExecution; });

				if (mStopExecution && mTasks.empty())
					return;

				task = std::move(mTasks.front());
				mTasks.pop_front();
			}

			task();
		}
	}
}