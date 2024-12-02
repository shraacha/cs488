#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
    public:
        ThreadPool(unsigned int numThreads = 1);

        void start();
        void queueJob(const std::function<void()> & job);
        void stop();
        bool isBusy();

    private:
        void threadLoop();

        const unsigned int m_numThreads;
        bool m_terminate;

        std::mutex m_queueMutex;
        std::condition_variable m_mutexCondition; // this is used to notify other threads that jobs are available
        std::queue<std::function<void()>> m_jobQueue;

        std::vector<std::thread> m_threads;
};
