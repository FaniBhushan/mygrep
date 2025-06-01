#pragma once
#include <vector>
#include <filesystem>
#include <thread>
#include <atomic>
#include "../headers/ConcurrentQueue.hpp"

namespace mygrep {
    class ThreadManager {
    public:
        ThreadManager(const std::vector<std::filesystem::path>& inputs);
        void run();
    private:
        ConcurrentQueue<std::filesystem::path> m_workQueue;
        std::vector<std::thread> m_workers;
        std::atomic<size_t> m_activeWorkers{0};
        void worker();
    };
}