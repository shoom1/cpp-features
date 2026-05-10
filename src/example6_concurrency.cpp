/**
 * Example 6: Thread Lifecycle and Cancellation
 *
 * This example intentionally has a shorter version span:
 * - C++11: std::thread with manual join and shared cancellation state
 * - C++20: std::jthread with automatic joining and stop_token
 *
 * Task: Run a background worker and stop it cleanly.
 */

#include <atomic>
#include <chrono>
#include <print>
#include <thread>

// ============================================================================
// C++11 Style: std::thread and manual lifecycle management
// ============================================================================

namespace cpp11_style {

class BackgroundWorker {
    std::atomic_bool stop_requested_{false};
    std::atomic_int completed_batches_{0};
    std::thread worker_;

public:
    BackgroundWorker() = default;

    void start() {
        worker_ = std::thread([this] {
            while (!stop_requested_.load()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                ++completed_batches_;
            }
        });
    }

    void requestStop() {
        stop_requested_.store(true);
    }

    void join() {
        if (worker_.joinable()) {
            worker_.join();
        }
    }

    int completedBatches() const {
        return completed_batches_.load();
    }

    ~BackgroundWorker() {
        requestStop();
        join();
    }

    BackgroundWorker(const BackgroundWorker&) = delete;
    BackgroundWorker& operator=(const BackgroundWorker&) = delete;
};

void demo() {
    std::println("=== C++11 Style: std::thread ===\n");

    BackgroundWorker worker;
    worker.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    worker.requestStop();
    worker.join();

    std::println("Completed batches: {}", worker.completedBatches());
    std::println("Caller owns cancellation state and must remember to join.");
    std::println("");
}

} // namespace cpp11_style

// ============================================================================
// C++20 Style: jthread and stop_token
// ============================================================================

namespace cpp20_style {

void demo() {
    std::println("=== C++20 Style: jthread and stop_token ===\n");

    std::atomic_int completed_batches{0};

    std::jthread worker([&](std::stop_token stop) {
        while (!stop.stop_requested()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            ++completed_batches;
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    worker.request_stop();

    std::println("Completed batches: {}", completed_batches.load());
    std::println("jthread requests cooperative stop and joins in its destructor.");
    std::println("");
}

} // namespace cpp20_style

// ============================================================================
// Main
// ============================================================================

int main() {
    cpp11_style::demo();
    cpp20_style::demo();

    return 0;
}
