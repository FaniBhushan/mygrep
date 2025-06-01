#pragma once
#include <filesystem>

namespace mygrep {
    class PatternSearcher {
    public:
        PatternSearcher() = default;

        void searchInFile(const std::filesystem::path &filePath) const;
    };
}
