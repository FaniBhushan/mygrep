//
// Created by fbhusha on 5/31/25.
//

#ifndef FILEQUEUE_H
#define FILEQUEUE_H
#include <mutex>
#include <queue>
#include <condition_variable>
#include <filesystem>

class ThreadSafeQueue {
public:
    // Add a file to the queue
    void push(const std::filesystem::path& file) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(file);
        }
        m_cv.notify_one(); // Notify all waiting threads that a new file is available
    }

    // Remove and return a file from the queue (with timeout)
    bool pop(std::filesystem::path& item, int timeout_ms = 10) {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (!m_cv.wait_for(lock, std::chrono::milliseconds(timeout_ms), [this] { return !m_queue.empty(); })) {
            return false;
        }
        item = m_queue.front();
        m_queue.pop();
        return true;
    }

    // Check if the queue is empty
    [[nodiscard]] bool empty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    // Get the size of the queue
    [[nodiscard]] size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

private:
    // Remove and return a file from the queue (blocking) -- now private to avoid accidental use
    bool pop(std::filesystem::path& item) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [this] { return !m_queue.empty(); }); // Wait until the queue is not empty
        if (m_queue.empty()) {
            return false;
        }
        item = m_queue.front();
        m_queue.pop();
        return true;
    }

    std::queue<std::filesystem::path> m_queue; // Queue to hold items of type path
    mutable std::mutex m_mutex; // Mutex for thread safety
    std::condition_variable m_cv; // Condition variable for synchronization
};

#endif //FILEQUEUE_H
