#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include "scheduler.h"

template <typename T>
class ThreadPool {
   public:
    // initialize the number of concurrency threads
    ThreadPool(size_t = std::thread::hardware_concurrency());

    // destroy thread pool and all created threads
    ~ThreadPool();

    // enqueue new thread task
    template <class F, class... Args>
    decltype(auto) add(F&& f, Args&&... args);

    // enqueue new timed task
    template <class F, class... Args>
    decltype(auto) add_delayed(std::chrono::milliseconds delay, F&& f, Args&&... args);

    void cancel_delayed(size_t id);

   private:
    std::unique_ptr<Scheduler<T>> scheduler_;
    TimerEventScheduler timer_scheduler_;
    // thread list, stores all threads
    std::vector<std::thread> workers_;
};

// constructor initialize a fixed size of worker
template <typename T>
inline ThreadPool<T>::ThreadPool(size_t threads) : scheduler_(std::make_unique<T>()) {
    // initialize worker
    for (size_t i = 0; i < threads; i++)
        workers_.emplace_back([this] {
            for (;;) {
                if (!this->scheduler_->execute()) {
                    break;
                }
            }
        });

    for (size_t i = 0; i < threads; i++)
        workers_.emplace_back([this] {
            for (;;) {
                if (!this->timer_scheduler_.execute()) {
                    break;
                }
            }
        });
}

// Enqueue a new thread
// use variadic templates and tail return type
template <typename T>
template <class F, class... Args>
inline decltype(auto) ThreadPool<T>::add(F&& f, Args&&... args) {
    return scheduler_->add(std::forward<F>(f), std::forward<Args>(args)...);
}

template <typename T>
template <class F, class... Args>
inline decltype(auto) ThreadPool<T>::add_delayed(std::chrono::milliseconds delay, F&& f,
                                                 Args&&... args) {
    return timer_scheduler_.add(delay, std::forward<F>(f), std::forward<Args>(args)...);
}

template <typename T>
void ThreadPool<T>::cancel_delayed(size_t id) {
    timer_scheduler_.cancel(id);
}

// destroy everything
template <typename T>
inline ThreadPool<T>::~ThreadPool() {
    scheduler_->stop();
    timer_scheduler_.stop();

    // let all processes into synchronous execution, use c++11 new for-loop:
    // for(value:values)
    for (std::thread& worker : workers_) worker.join();
}

#endif