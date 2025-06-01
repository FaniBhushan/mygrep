//
// Created by fbhusha on 5/31/25.
//
#include <cmath>
#include <algorithm>
#include "../headers/ThreadManager.hpp"
#include "../headers/Config.hpp"
#include "../headers/DirectoryCrawler.hpp"
#include "../headers/PatternSearcher.hpp"
#include "../headers/utility.hpp"

std::mutex ioMutex;

using namespace mygrep;
using namespace std;

ThreadManager::ThreadManager(vector<filesystem::path> directories)
    : m_inputList(std::move(directories)),
      m_dirQueue(),
      m_fileQueue(),
      m_producersDone(false) {
}

void ThreadManager::run() {
    for (auto &inputPath: m_inputList) {
        if (is_directory(inputPath)) m_dirQueue.push(inputPath);
        else if (is_regular_file(inputPath)) m_fileQueue.push(inputPath);
    }

    size_t initialProducers = config::USE_RATIO_BASED_START
                                  ? static_cast<size_t>(std::round(config::INITIAL_PRODUCER_RATIO * config::THREAD_CAP))
                                  : 1;

    size_t initialConsumers = config::USE_RATIO_BASED_START
                                  ? static_cast<size_t>(std::round(config::THREAD_CAP - initialProducers))
                                  : 1;

    for (size_t i = 0; i < initialProducers; ++i) spawnProducer();
    for (size_t i = 0; i < initialConsumers; ++i) spawnConsumer();

    // Wait for all producers to finish
    joinAll(m_producers);
    m_producersDone = true; // Signal consumers to finish

    // Wait for all consumers to finish
    joinAll(m_consumers);
}

void ThreadManager::spawnProducer() {
    m_producers.emplace_back(DirectoryCrawler(m_dirQueue, m_fileQueue, m_producersDone));
}

void ThreadManager::spawnConsumer() {
    m_consumers.emplace_back(PatternSearcher(m_fileQueue, config::SEARCH_PATTERN, m_producersDone));
}

bool ThreadManager::anyProducerJoinable() const {
    for (auto &p: m_producers) {
        if (p.joinable()) return true;
    }
    return false;
}

void ThreadManager::joinAll(std::vector<std::thread> &threads) {
    for (auto &t: threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

void ThreadManager::cleanupFinishedThreads(std::vector<std::thread> &threads) {
    threads.erase(
        std::remove_if(threads.begin(), threads.end(),
                       [](std::thread &t) {
                           if (t.joinable() && t.get_id() != std::this_thread::get_id()) {
                               t.join();
                               return true; // Remove the thread if it has finished
                           }
                           return false;
                       }),
        threads.end());
}
