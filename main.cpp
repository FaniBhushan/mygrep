#include <iostream>
#include <fstream>

#include "headers/Config.hpp"
#include "headers/ThreadManager.hpp"
#include <filesystem>
#include <vector>

#include "headers/utility.hpp"

using namespace mygrep;
using namespace std;


int main(int argc, char *argv[]) {
    namespace fs = std::filesystem;

    bool recursive = true;
    bool caseSensitive = true;
    bool useRatioBased = false;
    bool useRegex = false;
    string searchPattern;
    vector<fs::path> targets;

    // Parse options
    int argi = 1;
    if (argc < 2) {
        usage();
        return 1;
    }

    string opt = argv[argi];
    if (opt[0] == '-') {
        for (char c: opt.substr(1)) {
            if (c == 'c' || c == 'C') recursive = false;
            else if (c == 'i' || c == 'I') caseSensitive = false;
            else if (c == 'r' || c == 'R') useRegex = true;
            else if (c == 'p' || c == 'P') useRatioBased = true;
            else if (c == 'h') {
                usage();
                return 0;
            } else {
                cerr << "Unknown option: -" << c << endl;
                usage();
                return 1;
            }
        }
        ++argi;
    }

    if (argi >= argc) {
        usage();
        return 1;
    }

    searchPattern = argv[argi++];
    if (argi == argc) {
        targets.emplace_back(fs::current_path());
    } else {
        for (; argi < argc; ++argi) {
            if (argv[argi])
                targets.emplace_back(argv[argi]);
        }
    }

    vector<fs::path> validTargets;
    for (const auto &target: targets) {
        try {
            fs::path absPath = fs::absolute(target);
            if (!fs::exists(absPath)) {
                logError("Invalid path: " + absPath.string());
                continue;
            }
            if (fs::is_regular_file(absPath) && !isTextFile(absPath)) {
                continue;
            }
            validTargets.push_back(absPath);
        } catch (const fs::filesystem_error &e) {
            if (e.code() == std::errc::permission_denied) {
                logError("Permission denied: " + string(target));
            } else {
                logError("Error resolving path " + string(target) + ": " + e.what());
            }
        }
    }
    if (validTargets.empty()) {
        logError("No valid files or directories to search.");
        return 1;
    }

    // Set config
    config::SEARCH_PATTERN = searchPattern;
    config::USE_RATIO_BASED_START = useRatioBased;
    config::SEARCH_RECURSIVELY = recursive;
    config::CASE_SENSITIVE = caseSensitive;
    config::USE_REGEX = useRegex;

    ThreadManager manager(validTargets);
    manager.run();

    return 0;
}
