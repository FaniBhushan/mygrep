#pragma once
#include <queue>
#include <mutex>

// A simple thread-safe queue implementation using mutex for synchronization

namespace mygrep {
    template<typename T>
    class ConcurrentQueue {
        std::queue<T> q;
        std::mutex m;
    public:
        void push(const T& value) {
            std::lock_guard<std::mutex> lock(m);
            q.push(value);
        }
        bool try_pop(T& value) {
            std::lock_guard<std::mutex> lock(m);
            if (q.empty()) return false;
            value = q.front();
            q.pop();
            return true;
        }
        bool empty() {
            std::lock_guard<std::mutex> lock(m);
            return q.empty();
        }
    };
}