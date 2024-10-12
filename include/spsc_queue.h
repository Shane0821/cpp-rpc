#ifndef _SPSC_QUEUE_H
#define _SPSC_QUEUE_H

#include <atomic>
#include <memory>

// Simple lock-free single-producer single-consumer queue
template <typename T, size_t Capacity>
class SPSCQueue : private std::allocator<T> {
   public:
    SPSCQueue() {
        data_ = std::allocator_traits<std::allocator<T>>::allocate(*this, Capacity);
    }

    // non-copyable
    SPSCQueue(const SPSCQueue &) = delete;
    SPSCQueue &operator=(const SPSCQueue &) = delete;

    ~SPSCQueue() {
        for (size_t i = head_.load(std::memory_order_acquire);
             i != tail_.load(std::memory_order_acquire); i = (i + 1) % Capacity) {
            std::allocator_traits<std::allocator<T>>::destroy(*this, data_ + i);
        }
        std::allocator_traits<std::allocator<T>>::deallocate(*this, data_, Capacity);
    }

    template <typename... Args>
    bool emplace(Args &&...args) noexcept(
        std::is_nothrow_constructible<T, Args &&...>::value) {
        static_assert(std::is_constructible<T, Args &&...>::value,
                      "T must be constructible with Args&&...");

        size_t t = tail_.load(std::memory_order_relaxed);
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

        size_t h = head_.load(std::memory_order_relaxed);
        if (h == tail_.load(std::memory_order_acquire)) {  // (3)
            return false;
        }
        result = std::move(data_[h]);
        std::allocator_traits<std::allocator<T>>::destroy(*this, data_ + h);
        head_.store((h + 1) % Capacity, std::memory_order_release);  // (4)
        return true;
    }

    size_t size() const noexcept {
        size_t diff =
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
    std::atomic<size_t> head_{0};
    std::atomic<size_t> tail_{0};
};

#endif  // _SPSC_QUEUE_H