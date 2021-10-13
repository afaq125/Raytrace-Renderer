#pragma once

namespace Renderer
{
	class ThreadPool
	{
	public:
        explicit ThreadPool(
            const std::function<void(const Size)>& callable,
            const std::function<void()>& callback = []() {},
            const Size chunk_size = 10u,
			const Size threads = static_cast<const Size>(std::thread::hardware_concurrency())) :
			m_callable(callable),
			m_callback(callback),
            m_chunk_size(chunk_size),
			m_thread_count(threads),
			m_done(false),
			m_counter(0)
		{
		}
		ThreadPool(const ThreadPool &rhs) = delete;
		ThreadPool(ThreadPool &&rhs) = delete;
		ThreadPool& operator=(const ThreadPool& rhs) = delete;
		~ThreadPool() = default;

		bool operator()(const Size iterations)
		{
			m_iterations = iterations;

			auto callbackThread = std::thread(&ThreadPool::Callback, this);

			for (Size i = 0; i < m_thread_count - 1; ++i)
			{
				m_threads.push_back(std::thread(&ThreadPool::Job, this, 10u));
			}

			for (auto& thread : m_threads)
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
            const Size chunk_size = 10u,
			const Size threads = static_cast<const Size>(std::thread::hardware_concurrency()))
		{
			ThreadPool pool(callable, []() {}, chunk_size, threads);
			pool(iterations);
		}

		static void RunWithCallback(
			std::function<void(Size)> callable,
			std::function<void()> callback,
			const Size iterations,
            const Size chunk_size = 10u,
			const Size threads = static_cast<const Size>(std::thread::hardware_concurrency()))
		{
			ThreadPool pool(callable, callback, chunk_size, threads);
			pool(iterations);
		}

	private:
		void Job(const Size amount)
		{
            std::unique_lock<std::mutex> lock(m_mutex);

			while (!m_done)
			{
				const Size start = m_counter;
				Size end = m_counter + amount;
				m_counter += amount;

				if (end > m_iterations)
				{
					end = m_iterations;
					m_counter = m_iterations;
					m_done = true;
				}

                lock.unlock();

				for (Size i = start; i < end; ++i)
				{
					m_callable(i);
				}

                lock.lock();
			}
		};

		void Callback()
		{
			while (!m_done)
			{
				m_callback();
			}
		}

		const std::function<void(const Size)>& m_callable;
		const std::function<void()>& m_callback;
        const Size m_chunk_size;
		const Size m_thread_count;
		Size m_iterations;

		std::vector<std::thread> m_threads;
		std::mutex m_mutex;
		std::atomic_bool m_done;
		std::atomic_size_t m_counter;
	};
}