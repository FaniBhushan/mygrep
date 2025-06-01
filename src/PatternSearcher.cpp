//
// Created by fbhusha on 5/31/25.
//
#include <fstream>
#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <regex>

#include "../headers/PatternSearcher.hpp"

#include "../headers/Config.hpp"
#include "../headers/utility.hpp"

using namespace std;
using namespace mygrep;

PatternSearcher::PatternSearcher(ThreadSafeQueue &fileQueue,
                                 std::string pattern, std::atomic<bool> &done)
    : m_fileQueue(fileQueue), m_searchPattern(std::move(pattern)), m_producersDone(done) {
}

void PatternSearcher::operator()() {
    while (true) {
        filesystem::path filePath;
        // Use pop with timeout to allow periodic exit checks
        if (m_fileQueue.pop(filePath, 10)) {
            searchInFile(filePath);
        } else {
            // If no file and producers are done, exit
            if (m_producersDone && m_fileQueue.empty()) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void PatternSearcher::searchInFile(const filesystem::path &filePath) const {
    try {
        filesystem::path absPath = absolute(filePath);
        std::ifstream file(absPath);
        if (!file.is_open()) {
            logError("Error opening file: " + filePath.string());
            return;
        }

        std::string line;
        size_t lineNumber = 0;

        std::regex regexPattern;
        if (config::USE_REGEX) {
            std::regex::flag_type flags = std::regex::ECMAScript;
            if (!config::CASE_SENSITIVE) flags |= std::regex::icase;
            regexPattern = std::regex(m_searchPattern, flags);
        }

        while (std::getline(file, line)) {
            ++lineNumber;
            if (config::USE_REGEX) {
                auto begin = std::sregex_iterator(line.begin(), line.end(), regexPattern);
                auto end = std::sregex_iterator();
                for (auto it = begin; it != end; ++it) {
                    size_t match_pos = it->position();
                    size_t match_len = it->length();
                    printMatchSnippet(line, match_pos, match_len, lineNumber, filePath.string());
                }
            } else if (config::CASE_SENSITIVE) {
                size_t match_pos = std::string::npos;
                size_t searchStart = 0;
                while ((match_pos = line.find(m_searchPattern, searchStart)) != std::string::npos) {
                    printMatchSnippet(line, match_pos, m_searchPattern.size(), lineNumber, filePath.string());
                    searchStart = match_pos + m_searchPattern.size();
                }
            } else {
                auto searchStart = line.begin();
                while (searchStart != line.end()) {
                    auto it = std::search(
                        searchStart, line.end(),
                        config::SEARCH_PATTERN.begin(), config::SEARCH_PATTERN.end(),
                        [](char a, char b) {
                            return std::tolower(static_cast<unsigned char>(a)) == std::tolower(
                                       static_cast<unsigned char>(b));
                        }
                    );
                    if (it != line.end()) {
                        printMatchSnippet(line, std::distance(line.begin(), it), m_searchPattern.size(), lineNumber,
                                          filePath.string());
                        searchStart = it + m_searchPattern.size();
                    } else {
                        break;
                    }
                }
            }
        }
        file.close();
    } catch (const std::exception &e) {
        mygrep::logError("Error searching file " + filePath.string() + ": " + e.what());
    }
}
