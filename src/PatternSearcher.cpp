#include <fstream>
#include <filesystem>
#include <regex>
#include "../headers/PatternSearcher.hpp"
#include "../headers/Config.hpp"
#include "../headers/utility.hpp"

using namespace std;
namespace fs = std::filesystem;
using namespace mygrep;

std::mutex ioMutex;

// Function to print a snippet of the line where a match was found
void PatternSearcher::searchInFile(const fs::path &filePath) const {
    try {
        if (config::SEARCH_PATTERN.empty()) {
            // Handle gracefully: return, throw, or skip processing
            return;
        }
        fs::path absPath = fs::absolute(filePath);
        std::ifstream file(absPath);
        if (!file.is_open()) {
            logError("Error opening file: " + filePath.string());
            return;
        }

        std::string line;
        size_t lineNumber = 0;

        std::regex regexPattern;
        // Prepare regex if needed
        if (config::USE_REGEX) {
            std::regex::flag_type flags = std::regex::ECMAScript;
            if (!config::CASE_SENSITIVE) flags |= std::regex::icase;
            regexPattern = std::regex(config::SEARCH_PATTERN, flags);
        }

        // Read file line by line
        while (std::getline(file, line)) {
            ++lineNumber;
            if (config::USE_REGEX) {
                // Regex search
                auto begin = std::sregex_iterator(line.begin(), line.end(), regexPattern);
                auto end = std::sregex_iterator();
                for (auto it = begin; it != end; ++it) {
                    size_t match_pos = it->position();
                    size_t match_len = it->length();
                    printMatchSnippet(line, match_pos, match_len, lineNumber, filePath.string());
                }
            } else if (config::CASE_SENSITIVE) {
                // Case-sensitive substring search
                size_t match_pos = std::string::npos;
                size_t searchStart = 0;
                while ((match_pos = line.find(config::SEARCH_PATTERN, searchStart)) != std::string::npos) {
                    printMatchSnippet(line, match_pos, config::SEARCH_PATTERN.size(), lineNumber, filePath.string());
                    searchStart = match_pos + config::SEARCH_PATTERN.size();
                }
            } else {
                // Case-insensitive substring search
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
                        printMatchSnippet(line, std::distance(line.begin(), it), config::SEARCH_PATTERN.size(), lineNumber,
                                          filePath.string());
                        searchStart = it + config::SEARCH_PATTERN.size();
                    } else {
                        break;
                    }
                }
            }
        }
        file.close();
    } catch (const std::exception &e) {
        logError("Error searching file " + filePath.string() + ": " + e.what());
    }
}
