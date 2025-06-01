//
// Created by fbhusha on 6/1/25.
//

#ifndef LOGGING_H
#define LOGGING_H

#include <mutex>
#include <iostream>
#include <fstream>
#include <algorithm>

extern std::mutex ioMutex;

namespace mygrep {
    inline void logInfo(const std::string &message) {
        std::lock_guard<std::mutex> lock(ioMutex);
        std::cout << message << std::endl;
    }

    inline void logError(const std::string &message) {
        std::lock_guard<std::mutex> lock(ioMutex);
        std::cerr << message << std::endl;
    }

    inline void usage() {
        logInfo(
            "Usage: mygrep [-options] <search_pattern> <list of files/dirs>\n"
            "Options(any combination of below):\n"
            "  c,C    search only current directory\n"
            "  i,I    Ignore case in search\n"
            "  r,R    Use ratio based initial threads\n"
            "  h,     Show this help message\n"
        );
    }

    inline bool isTextFile(const std::filesystem::path& path) {
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
    }

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
#endif //LOGGING_H
