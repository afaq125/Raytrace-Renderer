#pragma once

namespace Renderer
{
	class ThreadPool
	{
	public:
		explicit ThreadPool(
			const std::function<void(const Size)>& callable,
			const std::function<void()>& callback = []() {},
			const Size threads = static_cast<const Size>(std::thread::hardware_concurrency())) :
			mCallable(callable),
			mCallback(callback),
			mThreadCount(threads),
			mDone(false),
			mCounter(0)
		{
		}
		ThreadPool(const ThreadPool &rhs) = delete;
		ThreadPool(ThreadPool &&rhs) = delete;
		ThreadPool& operator=(const ThreadPool& rhs) = delete;
		~ThreadPool() = default;

		bool operator()(const Size iterations)
		{
			mIterations = iterations;

			auto callbackThread = std::thread(&ThreadPool::Callback, this);

			for (Size i = 0; i < mThreadCount - 1; ++i)
			{
				mThreads.push_back(std::thread(&ThreadPool::Job, this, 10u));
			}

			for (auto&& thread : mThreads)
			{
				if (thread.joinable())
				{
					thread.join();
				}
			}

			callbackThread.join();

			return true;
		}

		static void Run(
			std::function<void(Size)> callable,
			const Size iterations,
			const Size threads = static_cast<const Size>(std::thread::hardware_concurrency()))
		{
			ThreadPool pool(callable, []() {}, threads);
			pool(iterations);
		}

		static void RunWithCallback(
			std::function<void(Size)> callable,
			std::function<void()> callback,
			const Size iterations,
			const Size threads = static_cast<const Size>(std::thread::hardware_concurrency()))
		{
			ThreadPool pool(callable, callback, threads);
			pool(iterations);
		}

	private:
		void Job(const Size amount)
		{
			while (!mDone)
			{
				mMutex.lock();

				const Size start = mCounter;
				Size end = mCounter + amount;
				mCounter += static_cast<int>(amount);

				if (end > mIterations)
				{
					end = mIterations;
					mCounter = static_cast<int>(mIterations);
					mDone = true;
				}

				mMutex.unlock();

				for (Size i = start; i < end; ++i)
				{
					mCallable(i);
				}
			}
		};

		void Callback()
		{
			while (!mDone)
			{
				mCallback();
			}
		}

		const std::function<void(const Size)>& mCallable;
		const std::function<void()>& mCallback;
		const Size mThreadCount;
		Size mIterations;

		std::vector<std::thread> mThreads;
		std::mutex mMutex;
		std::atomic_bool mDone;
		std::atomic_int mCounter;
	};
}