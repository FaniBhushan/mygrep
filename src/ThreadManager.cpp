#include "../headers/ThreadManager.hpp"
#include "../headers/utility.hpp"
#include "../headers/PatternSearcher.hpp"
#include "../headers/Config.hpp"
#include <filesystem>
#include <iostream>

using namespace std;
namespace fs = std::filesystem;
using namespace mygrep;

ThreadManager::ThreadManager(const vector<fs::path> &inputs) {
    // Push all initial input paths (files or directories) to the work queue
    for (const auto &p: inputs) m_workQueue.push(p);
}

// main function to run the thread manager
void ThreadManager::run() {
    size_t numThreads = config::THREAD_CAP;

    // Launch worker threads equal to the configured thread capacity
    for (size_t i = 0; i < numThreads; ++i)
        m_workers.emplace_back(&ThreadManager::worker, this);

    // Wait for all worker threads to finish
    for (auto &t: m_workers) t.join();
}

// Worker function that processes paths from the queue
void ThreadManager::worker() {
    PatternSearcher searcher;
    fs::path path;
    while (true) {
        // Try to get a path from the queue
        if (!m_workQueue.try_pop(path)) {
            // If no work, check if all workers are idle and queue is empty
            if (m_activeWorkers == 0 && m_workQueue.empty()) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        m_activeWorkers++;
        try {
            if (fs::is_directory(path)) {
                // If it's a directory, push its contents (files and subdirs) to the queue
                for (auto &entry: fs::directory_iterator(path)) {
                    //Push file only if its a text file with ASCII chars
                    if (fs::is_regular_file(entry) && isTextFile(entry)) {
                        m_workQueue.push(entry);
                    } else if (fs::is_directory(entry)) {
                        m_workQueue.push(entry);
                    }
                }
            } else if (fs::is_regular_file(path) && isTextFile(path)) {
                // If it's a file, search for the pattern
                searcher.searchInFile(path);
            }
        } catch (const std::exception &e) {
            logError("Error accessing " + path.string() + ": " + e.what());
        }
        m_activeWorkers--;
    }
}
