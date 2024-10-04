#ifndef _SPSC_QUEUE_H
#define _SPSC_QUEUE_H

#include <atomic>
#include <memory>

// multi-producer multi-consumer queue
template <typename T, size_t Capacity>
class MPMCQueue : private std::allocator<T> {
   public:
    MPMCQueue() noexcept {
        data_ = std::allocator_traits<std::allocator<T>>::allocate(*this, Capacity);
    }

    // non-copyable
    MPMCQueue(const MPMCQueue &) = delete;
    MPMCQueue &operator=(const MPMCQueue &) = delete;

    ~MPMCQueue() {
        for (size_t i = 0; i < Capacity; ++i) {
            std::allocator_traits<std::allocator<T>>::destroy(*this, data_ + i);
        }
        std::allocator_traits<std::allocator<T>>::deallocate(*this, data_, Capacity);
    }

    template <typename... Args>
    bool emplace(Args &&...args) noexcept(
        std::is_nothrow_constructible<T, Args &&...>::value) {
        static_assert(std::is_constructible<T, Args &&...>::value,
                      "T must be constructible with Args&&...");

        size_t t, w;
        do {
            t = tail_.load(std::memory_order_relaxed);
            if ((t + 1) % Capacity == head_.load(std::memory_order_acquire)) {
                return false;
            }
        } while (!tail_.compare_exchange_weak(t, (t + 1) % Capacity,
                                              std::memory_order_relaxed));

        std::allocator_traits<std::allocator<T>>::construct(*this, data_ + t,
                                                            std::forward<Args>(args)...);

        do {
            w = t;
        } while (!write_.compare_exchange_weak(
            w, (w + 1) % Capacity, std::memory_order_release, std::memory_order_relaxed));
        return true;
    }

    bool pop(T &result) noexcept {
        static_assert(std::is_nothrow_destructible<T>::value,
                      "T must be nothrow destructible");

        size_t h;
        do {
            h = head_.load(std::memory_order_relaxed);
            if (h == write_.load(std::memory_order_acquire)) {
                return false;
            }
            result = std::move(data_[h]);
            std::allocator_traits<std::allocator<T>>::destroy(*this, data_ + h);
        } while (!head_.compare_exchange_strong(
            h, (h + 1) % Capacity, std::memory_order_release, std::memory_order_relaxed));
        return true;
    }

    size_t size() const noexcept {
        auto h = head_.load(std::memory_order_acquire);
        auto t = tail_.load(std::memory_order_acquire);
        return (t >= h) ? t - h : Capacity - h + t;
    }

    bool empty() const noexcept {
        return head_.load(std::memory_order_acquire) ==
               tail_.load(std::memory_order_acquire);
    }

   private:
    T *data_;  // queue data
    std::atomic<size_t> head_{0};
    std::atomic<size_t> tail_{0};
    std::atomic<size_t> write_{0};
};

#endif  // _SPSC_QUEUE_H