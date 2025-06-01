//
// Created by fbhusha on 5/31/25.
//

#ifndef DIRECTORYCRAWLER_H
#define DIRECTORYCRAWLER_H

#include <filesystem>
#include <atomic>

#include "ThreadSafeQueue.hpp"

namespace mygrep {
    class DirectoryCrawler {
    public:
        DirectoryCrawler(ThreadSafeQueue &dirQueue,
                         ThreadSafeQueue &fileQueue,
                         std::atomic<bool>& producersDone);

        void operator()();

    private:
        void scanDirectory(const std::filesystem::path &dir);

        ThreadSafeQueue &m_dirQueue;
        ThreadSafeQueue &m_fileQueue;
        std::atomic<bool>& m_producersDone;
    };
}
#endif //DIRECTORYCRAWLER_H
