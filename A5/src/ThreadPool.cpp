#include "ThreadPool.hpp"
#include <mutex>

ThreadPool::ThreadPool(unsigned int numThreads) : m_numThreads(numThreads), m_terminate(false) {}

void ThreadPool::start()
{
    for (unsigned int i = 0; i < m_numThreads; ++i)
    {
        m_threads.emplace_back(std::thread(&ThreadPool::threadLoop, this));
    }
}

void ThreadPool::queueJob(const std::function<void()> & job)
{
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_jobQueue.push(job);
    }

    m_mutexCondition.notify_one();
}

void ThreadPool::stop()
{
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_terminate = true;
    }

    m_mutexCondition.notify_all();
    for (std::thread& active_thread : m_threads) {
        active_thread.join();
    }
    m_threads.clear();
}

bool ThreadPool::isBusy()
{
    bool isBusy;
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        isBusy = !m_jobQueue.empty();
    }
    return isBusy;
}

void ThreadPool::threadLoop()
{
    while (true)
    {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_mutexCondition.wait(lock, [this]
                                 { return !m_jobQueue.empty() || m_terminate; });
            if (m_terminate)
            {
                return;
            }

            job = m_jobQueue.front();
            m_jobQueue.pop();
        }
        job();
    }
}
