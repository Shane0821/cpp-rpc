#ifndef _SPSC_QUEUE_H
#define _SPSC_QUEUE_H

#include <atomic>
#include <memory>

// Simple lock-free single-producer single-consumer queue
template <typename T, int Capacity>
class SPSCQueue : private std::allocator<T> {
   public:
    SPSCQueue() {
        data_ = std::allocator_traits<std::allocator<T>>::allocate(*this, Capacity);
    }

    // non-copyable and non-movable
    SPSCQueue(const SPSCQueue &) = delete;
    SPSCQueue &operator=(const SPSCQueue &) = delete;

    ~SPSCQueue() {
        for (int i = 0; i < Capacity; ++i) {
            std::allocator_traits<std::allocator<T>>::destroy(*this, data_ + i);
        }
        std::allocator_traits<std::allocator<T>>::deallocate(*this, data_, Capacity);
    }

    template <typename... Args>
    bool emplace(Args &&...args) noexcept(
        std::is_nothrow_constructible<T, Args &&...>::value) {
        static_assert(std::is_constructible<T, Args &&...>::value,
                      "T must be constructible with Args&&...");

        int t = tail_.load(std::memory_order_relaxed);
        if ((t + 1) % Capacity == head_.load(std::memory_order_acquire)) {  // (1)
            return false;
        }
        std::allocator_traits<std::allocator<T>>::construct(*this, data_ + t,
                                                            std::forward<Args>(args)...);
        // (2) synchronizes with (3)
        tail_.store((t + 1) % Capacity, std::memory_order_release);  // (2)
        return true;
    }

    bool pop(T &result) noexcept {
        static_assert(std::is_nothrow_destructible<T>::value,
                      "T must be nothrow destructible");

        int h = head_.load(std::memory_order_relaxed);
        if (h == tail_.load(std::memory_order_acquire)) {  // (3)
            return false;
        }
        result = std::move(data_[h]);
        std::allocator_traits<std::allocator<T>>::destroy(*this, data_ + h);
        head_.store((h + 1) % Capacity, std::memory_order_release);  // (4)
        return true;
    }

    int size() const noexcept {
        int diff =
            tail_.load(std::memory_order_acquire) - head_.load(std::memory_order_acquire);
        if (diff < 0) {
            diff += Capacity;
        }
        return diff;
    }

    bool empty() const noexcept {
        return head_.load(std::memory_order_acquire) ==
               tail_.load(std::memory_order_acquire);
    }

   private:
    T *data_;  // queue data
    std::atomic<int> head_{0};
    std::atomic<int> tail_{0};
};

#endif  // _SPSC_QUEUE_H