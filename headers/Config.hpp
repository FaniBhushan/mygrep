
#pragma once
#include <thread>


namespace config {
    const size_t THREAD_CAP = std::max(1u, std::thread::hardware_concurrency() - 1);; // Number of threads to use
    inline std::string SEARCH_PATTERN = "TODO";
    inline bool SEARCH_RECURSIVELY = true;
    inline bool CASE_SENSITIVE = true;
    inline bool USE_REGEX = false; // Use regex for pattern matching
}
