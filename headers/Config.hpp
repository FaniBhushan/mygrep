//
// Created by fbhusha on 5/31/25.
//

#ifndef CONFIG_H
#define CONFIG_H

#include <thread>


namespace config {
    const size_t THREAD_CAP = std::max(1u, std::thread::hardware_concurrency() - 1);; // Number of threads to use
    constexpr double INITIAL_PRODUCER_RATIO = 0.75; // Start with 75% of THREAD_CAP as producers
    constexpr double MIN_PRODUCER_RATIO = 0.20;     // Minimum 20% of THREAD_CAP as producers
    constexpr double PRODUCER_BOOST_THRESHOLD_RATIO = 0.25; // Boost if fileQueue_ > 25% of THREAD_CAP
    constexpr double CONSUMER_BOOST_STEP_RATIO = 0.125;     // Boost consumers by 12.5% of THREAD_CAP
    inline bool USE_RATIO_BASED_START = false; // Set by user input at runtime
    inline std::string SEARCH_PATTERN = "TODO";
    inline bool SEARCH_RECURSIVELY = true;
    inline bool CASE_SENSITIVE = true;
    inline bool USE_REGEX = false; // Use regex for pattern matching
}
#endif //CONFIG_H
