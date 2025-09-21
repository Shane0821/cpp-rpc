#ifndef _MPMC_QUEUE_H
#define _MPMC_QUEUE_H

#include <atomic>
#include <memory>

// multi-producer multi-consumer queue
template <typename T, size_t Capacity = 65536>
class MPMCQueue : private std::allocator<T> {
   public:
    MPMCQueue() noexcept {
        data_ = std::allocator_traits<std::allocator<T>>::allocate(*this, Capacity);
        ticket_ = new std::atomic<size_t>[Capacity];
        for (size_t i = 0; i < Capacity; ++i) {
            ticket_[i].store(0, std::memory_order_relaxed);
        }
    }

    // non-copyable
    MPMCQueue(const MPMCQueue &) = delete;
    MPMCQueue &operator=(const MPMCQueue &) = delete;

    ~MPMCQueue() {
        std::destroy(data_, data_ + Capacity);
        std::allocator_traits<std::allocator<T>>::deallocate(*this, data_, Capacity);
        delete[] ticket_;
    }

    template <typename... Args>
    void emplace(Args &&...args) noexcept(
        std::is_nothrow_constructible<T, Args &&...>::value) {
        static_assert(std::is_constructible<T, Args &&...>::value,
                      "T must be constructible with Args&&...");

        auto tail = tail_.fetch_add(1);  // tail: before increment
        auto id = idx(tail);
        while (turn(tail) * 2 != ticket_[id].load(std::memory_order_acquire));

        std::construct_at(data_ + id, std::forward<Args>(args)...);
        ticket_[id].store(turn(tail) * 2 + 1, std::memory_order_release);
    }

    void pop(T &result) noexcept {
        static_assert(std::is_nothrow_destructible<T>::value,
                      "T must be nothrow destructible");

        auto head = head_.fetch_add(1);
        auto id = idx(head);

        while (turn(head) * 2 + 1 != ticket_[id].load(std::memory_order_acquire));

        result = std::move(data_[id]);

        ticket_[id].store(turn(head) * 2 + 2, std::memory_order_release);
    }

    size_t size() const noexcept {
        return tail_.load(std::memory_order_acquire) -
               head_.load(std::memory_order_acquire);
    }

    bool empty() const noexcept {
        return head_.load(std::memory_order_acquire) ==
               tail_.load(std::memory_order_acquire);
    }

   private:
    constexpr size_t idx(size_t i) const noexcept { return i % Capacity; }

    constexpr size_t turn(size_t i) const noexcept { return i / Capacity; }

    T *data_;
    std::atomic<size_t> *ticket_;
    std::atomic<size_t> head_{0};
    std::atomic<size_t> tail_{0};
};

#endif  // _MPMC_QUEUE_H