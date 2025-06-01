#pragma once
#include <string>

#include "ThreadSafeQueue.hpp"

namespace mygrep {
    class PatternSearcher {
    public:
        PatternSearcher(ThreadSafeQueue &fileQueue,
                        std::string pattern, std::atomic<bool>& done);

        void operator()();

    private:
        void searchInFile(const std::filesystem::path& filePath) const;

        ThreadSafeQueue& m_fileQueue; // Queue to hold file paths
        std::string m_searchPattern; // Pattern to search for
        std::atomic<bool>& m_producersDone; // Flag to indicate if producers are done
    };
}
