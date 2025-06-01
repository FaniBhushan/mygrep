cmake_minimum_required(VERSION 3.28)
project(mygrep)

set(CMAKE_CXX_STANDARD 20)

add_executable(mygrep main.cpp
        headers/ThreadSafeQueue.hpp
        headers/DirectoryCrawler.hpp
        headers/PatternSearcher.hpp
        headers/ThreadManager.hpp
        headers/Config.hpp
        src/PatternSearcher.cpp
        src/DirectoryCrawler.cpp
        src/ThreadManager.cpp
        headers/utility.hpp
)
