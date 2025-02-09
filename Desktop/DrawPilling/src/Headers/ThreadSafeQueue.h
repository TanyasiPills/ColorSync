#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include "NewRenderer.h"
#include "Messages.h"

class ThreadSafeQueue {
public:
    void push(const DrawMessage& task) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(task);
        cv_.notify_one();
    }

    bool pop(DrawMessage& task) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return false;
        }
        task = queue_.front();
        queue_.pop();
        return true;
    }

private:
    std::queue<DrawMessage> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
};
