cmake_minimum_required(VERSION 3.28)
project(mygrep)

set(CMAKE_CXX_STANDARD 20)

add_executable(mygrep main.cpp
        headers/PatternSearcher.hpp
        headers/ThreadManager.hpp
        headers/Config.hpp
        src/PatternSearcher.cpp
        src/ThreadManager.cpp
        headers/utility.hpp
        headers/ConcurrentQueue.hpp
)

option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
    add_executable(mygrep_tests
            tests/unitTests/main.cpp
            tests/unitTests/PatternSearcher.test.hpp
            tests/unitTests/ThreadManager.test.hpp
        headers/PatternSearcher.hpp
        headers/ThreadManager.hpp
        headers/Config.hpp
        src/PatternSearcher.cpp
        src/ThreadManager.cpp
        headers/utility.hpp
        headers/ConcurrentQueue.hpp
    )
endif()