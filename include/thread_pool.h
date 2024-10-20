#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include <condition_variable>  // std::condition_variable
#include <functional>          // std::function, std::bind
#include <future>              // std::future, std::packaged_task
#include <memory>              // std::make_shared
#include <mutex>               // std::mutex, std::unique_lock
#include <queue>               // std::queue
#include <stdexcept>           // std::runtime_error
#include <thread>              // std::thread
#include <utility>             // std::move, std::forward
#include <vector>              // std::vector

class ThreadPool {
   public:
    // initialize the number of concurrency threads
    ThreadPool(size_t);
    // destroy thread pool and all created threads
    ~ThreadPool();

    // enqueue new thread task
    template <class F, class... Args>
    decltype(auto) enqueue(F&& f, Args&&... args);

   private:
    // thread list, stores all threads
    std::vector<std::thread> workers_;
    // queue task, the type of queue elements are functions with void return type
    std::queue<std::function<void()>> tasks_;

    // for synchonization
    std::mutex queue_mutex_;
    std::condition_variable condition_;

    bool stop_ = false;
};

// constructor initialize a fixed size of worker
inline ThreadPool::ThreadPool(size_t threads) : stop_(false) {
    // initialize worker
    for (size_t i = 0; i < threads; i++)
        workers_.emplace_back([this] {
            for (;;) {
                // define function task container, return type is void
                std::function<void()> task;

                // critical section
                {
                    // get mutex
                    std::unique_lock<std::mutex> lock(this->queue_mutex_);

                    // block current thread
                    // check predicate on wake up
                    // proceed if stop or task queue is not empty
                    this->condition_.wait(
                        lock, [this] { return this->stop_ || !this->tasks_.empty(); });

                    // return if queue empty and task finished
                    if (this->stop_ && this->tasks_.empty()) return;

                    // otherwise execute the first element of queue
                    task = std::move(this->tasks_.front());
                    this->tasks_.pop();
                }

                // execution
                task();
            }
        });
}

// Enqueue a new thread
// use variadic templates and tail return type
template <class F, class... Args>
decltype(auto) ThreadPool::enqueue(F&& f, Args&&... args) {
    // deduce return type
    using return_type = std::result_of<F(Args...)>::type;

    // fetch task
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();

    // critical section
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);

        // avoid add new thread if theadpool is destroyed
        if (stop_) throw std::runtime_error("enqueue on stopped ThreadPool");

        // add thread to queue
        // use lambda function to wrap the task
        // return type is void, no parameter
        // capture copies the task, ref count +1
        tasks_.emplace([task] { (*task)(); });
    }

    // notify a wait thread
    condition_.notify_one();
    return res;
}

// destroy everything
inline ThreadPool::~ThreadPool() {
    // critical section
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }

    // wake up all threads
    condition_.notify_all();

    // let all processes into synchronous execution, use c++11 new for-loop:
    // for(value:values)
    for (std::thread& worker : workers_) worker.join();
}

#endif