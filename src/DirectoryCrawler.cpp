//
// Created by fbhusha on 5/31/25.
//

#include <iostream>
#include "../headers/DirectoryCrawler.hpp"

#include "../headers/Config.hpp"
#include "../headers/utility.hpp"

using namespace std;
using namespace mygrep;

DirectoryCrawler::DirectoryCrawler(ThreadSafeQueue &dirQueue,
                                   ThreadSafeQueue &fileQueue,
                                   std::atomic<bool>& producersDone)
    : m_dirQueue(dirQueue), m_fileQueue(fileQueue), m_producersDone(producersDone) {
}

void DirectoryCrawler::operator()() {
    int emptyCount = 0;
    const int maxEmpty = 50; // 50 * 10ms = 0.5s
    while (true) {
        filesystem::path dirPath;
        if (m_dirQueue.pop(dirPath, 10)) {
            scanDirectory(dirPath);
            emptyCount = 0;
        } else {
            ++emptyCount;
            if (emptyCount >= maxEmpty) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void DirectoryCrawler::scanDirectory(const filesystem::path &dir) {
    try {
        for (const auto &entry : filesystem::directory_iterator(dir)) {
            if (entry.is_directory() && config::SEARCH_RECURSIVELY) {
                m_dirQueue.push(entry.path());
            } else if (entry.is_regular_file()) {
                if (isTextFile(entry.path())) {
                    m_fileQueue.push(entry.path());
                }
            }
        }
    } catch (const filesystem::filesystem_error &e) {
        logError("Error accessing directory "
    + dir.string() + ": " + e.what());
    }
}
