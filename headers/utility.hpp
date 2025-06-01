
#pragma once

#include <mutex>
#include <iostream>
#include <fstream>
#include <algorithm>

extern std::mutex ioMutex;

namespace mygrep {

    // Utility functions for logging and file handling

    // Log an informational message to standard output in a thread-safe manner
    inline void logInfo(const std::string &message) {
        std::lock_guard<std::mutex> lock(ioMutex);
        std::cout << message << std::endl;
    }

    // Log an error message to standard error in a thread-safe manner
    inline void logError(const std::string &message) {
        std::lock_guard<std::mutex> lock(ioMutex);
        std::cerr << message << std::endl;
    }

    // prints the usage help message
    inline void usage() {
        logInfo(
            "Usage: mygrep [-options] <search_pattern> <list of files/dirs>\n"
            "Options(any combination of below):\n"
            "  c,C    search only current directory\n"
            "  i,I    Ignore case in search\n"
            "  r,R    regex based search\n"
            "  h,     Show this help message\n"
        );
    }

    // Check if a file is a text file by reading a sample of its content
    inline bool isTextFile(const std::filesystem::path& path) {
        try {
            std::ifstream file(path, std::ios::binary);
            if (!file) return false;
            const size_t sampleSize = 512;
            char buffer[sampleSize];
            file.read(buffer, sampleSize);
            std::streamsize bytesRead = file.gcount();
            int nonAscii = 0;
            for (std::streamsize i = 0; i < bytesRead; ++i) {
                unsigned char c = buffer[i];
                if ((c < 32 && c != 9 && c != 10 && c != 13) || c > 126) {
                    ++nonAscii;
                }
            }
            return bytesRead > 0 && (nonAscii < bytesRead * 0.05);
        } catch (const std::ios_base::failure& e) {
            if (e.code() == std::errc::permission_denied) {
                throw std::runtime_error("Permission denied: " + path.string());
            } else {
                logError("Error opening file: " + path.string() + " (" + e.what() + ")");
                return false;
            }
        }
    }

    // Print a snippet of the line where a match was found. prints 20 characters before and after the match with ellipsis if needed in the beginning or end.
    inline void printMatchSnippet(const std::string& line, size_t matchPos, size_t patternLen,size_t lineNumber, std::string filePath) {
        size_t start = (matchPos > 20) ? matchPos - 20 : 0;
        size_t end = std::min(line.size(), matchPos + patternLen + 20);

        std::string snippet;
        if (start > 0) snippet += "...";
        snippet += line.substr(start, end - start);
        if (end < line.size()) snippet += "...";
        logInfo("[MATCH] " + filePath +" : "+ std::to_string(lineNumber) + " : " + snippet);
    }
}

