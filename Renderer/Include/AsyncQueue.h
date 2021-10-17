#pragma once

namespace Renderer
{
    template <typename T>
    class AsyncQueue
    {
    public:
        AsyncQueue(std::function<void(const T&)> callable) :
            m_callable(std::move(callable)),
            m_run(true)
        {
            m_thread = std::thread(&AsyncQueue::Work, this);
        }
        ~AsyncQueue()
        {
            m_run = false;
            m_conditional.notify_one();

            m_thread.join();
        }

        void Push(const T& item)
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            m_queue.push(item);

            lock.unlock();
            m_conditional.notify_one();
        }

        bool Empty()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_queue.empty();
        }

    private:
        void Work()
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            while (m_run)
            {
                m_conditional.wait(lock, [&](){ return !m_queue.empty(); });

                if (!m_queue.empty())
                {
                    T value = m_queue.front();
                    m_queue.pop();

                    lock.unlock();

                    m_callable(value);

                    lock.lock();
                }
            }

            while (!m_queue.empty())
            {
                T value = m_queue.front();
                m_queue.pop();

                m_callable(value);
            }
        }

        std::thread m_thread;
        std::mutex m_mutex;
        std::condition_variable m_conditional;
        std::atomic_bool m_run;

        std::queue<T> m_queue;
        std::function<void(const T&)> m_callable;
    };
}