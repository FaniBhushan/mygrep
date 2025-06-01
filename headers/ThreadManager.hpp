//
// Created by fbhusha on 5/31/25.
//

#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <thread>
#include <vector>
#include <atomic>
#include <filesystem>
#include "ThreadSafeQueue.hpp"

namespace mygrep {

    class ThreadManager {

    public:
        ThreadManager(std::vector<std::filesystem::path> directories);

        void run();

    private:
        ThreadSafeQueue m_dirQueue; // Queue for directories to crawl
        ThreadSafeQueue m_fileQueue; // Queue for files to search
        std::atomic<bool> m_producersDone; // Flag to indicate if producers are done
        std::vector<std::thread> m_producers; // Threads for directory crawling
        std::vector<std::thread> m_consumers; // Threads for pattern searching
        std::vector<std::filesystem::path> m_inputList; // Directories to crawl

        void spawnProducer();
        void spawnConsumer();
        bool anyProducerJoinable() const;
        void joinAll(std::vector<std::thread>& threads);
        void cleanupFinishedThreads(std::vector<std::thread>& threads);
    };

} // namespace mygrep


#endif //THREADMANAGER_H
