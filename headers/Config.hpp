
#pragma once
#include <thread>


namespace config {
    // Number of threads to use. This is one less than the number of available cores to avoid overloading the system.
    const size_t THREAD_CAP = std::max(1u, std::thread::hardware_concurrency() - 1);

    // Default Configurations for the search pattern and options
    inline std::string SEARCH_PATTERN = "";
    inline bool SEARCH_RECURSIVELY = true; // Search recursively in directories
    inline bool CASE_SENSITIVE = true; // For Case-sensitive search
    inline bool USE_REGEX = false; // Use regex for pattern matching
}
