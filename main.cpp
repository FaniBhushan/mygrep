// main.cpp
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include "headers/Config.hpp"
#include "headers/ThreadManager.hpp"
#include "headers/utility.hpp"

using namespace mygrep;
using namespace std;

int main(int argc, char *argv[]) {
    namespace fs = std::filesystem;

    // Default options
    bool recursive = true;
    bool caseSensitive = true;
    bool useRegex = false;
    string searchPattern;
    vector<fs::path> targets;

    // Parse command-line options
    int argi = 1;
    if (argc < 2) {
        usage();
        return 1;
    }

    // Parse options from first argument if present
    string opt = argv[argi];
    if (opt[0] == '-') {
        for (char c: opt.substr(1)) {
            if (c == 'c' || c == 'C') recursive = false; // Only current directory
            else if (c == 'i' || c == 'I') caseSensitive = false; // Ignore case
            else if (c == 'r' || c == 'R') useRegex = true; // Use regex
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

    // Get search pattern from arguments
    searchPattern = argv[argi++];
    if (searchPattern.empty()) {
        cerr << "No search pattern provided." << endl;
        usage();
        return 1;
    }

    // Get targets (files/directories) from arguments or use current directory
    if (argi == argc) {
        targets.emplace_back(fs::current_path());
    } else {
        for (; argi < argc; ++argi) {
            if (argv[argi])
                targets.emplace_back(argv[argi]);
        }
    }

    // Validate targets: must exist and be text files or directories
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
                logError("Permission denied: " + target.string());
            } else {
                logError("Error resolving path " + target.string() + ": " + e.what());
            }
        }
    }
    if (validTargets.empty()) {
        logError("Restricted or No valid files or directories to search.");
        return 1;
    }

    // Set config for the rest of the program
    config::SEARCH_PATTERN = searchPattern;
    config::SEARCH_RECURSIVELY = recursive;
    config::CASE_SENSITIVE = caseSensitive;
    config::USE_REGEX = useRegex;

    // Start threaded search
    ThreadManager manager(validTargets);
    manager.run();

    return 0;
}
