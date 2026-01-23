#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <tuple>
#include <vector>

#include "common.hpp"
#include "config/worker_config.hpp"
#include "logging/logging.hpp"

namespace tur
{
	class WorkerPool
	{
		using task_t = std::function<void()>;

	public:
		~WorkerPool();

	public:
		void initialize(const WorkerConfig& workerConfig);

		void poll_tasks();

	public:
		template <typename ReturnType>
		void submit(std::function<ReturnType()> task, std::function<void(ReturnType)> callback = {})
		{
			{
				std::lock_guard<std::mutex> lock(mQueueMutex);

				mTasks.emplace_back(
					[this, callback = std::move(callback), task = std::move(task)]()
					{
						ReturnType returnValue = task();

						if(callback)
						{
							std::lock_guard<std::mutex> lockCallback(mCallbackMutex);
							mCallbacks.push_back([callback, returnValue = std::move(returnValue)]() { callback(returnValue); });
						}
					}
				);
			}

			mConditionalVar.notify_one();
		}

	private:
		void worker_function();

	private:
		std::vector<std::thread> mThreads;
		std::vector<task_t> mCallbacks;
		std::deque<task_t> mTasks;

		std::condition_variable mConditionalVar;
		std::mutex mQueueMutex;
		std::mutex mCallbackMutex;

		bool mStopExecution = false;
	};
}