#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <condition_variable>  // std::condition_variable
#include <functional>          // std::function, std::bind
#include <future>              // std::future, std::packaged_task
#include <memory>              // std::make_shared
#include <mutex>               // std::mutex, std::unique_lock
#include <queue>               // std::queue
#include <stdexcept>           // std::runtime_error
#include <thread>              // std::thread
#include <utility>             // std::move, std::forward

template <typename T>
class Scheduler {
   public:
    // enqueue new thread task
    template <class F, class... Args>
    decltype(auto) add(F&& f, Args&&... args) {
        return static_cast<T*>(this)->add(std::forward<F>(f),
                                          std::forward<Args>(args)...);
    }

    std::function<void()> get() { return static_cast<T*>(this)->get(); }

    void stop() { static_cast<T*>(this)->stop(); }
};

class FIFOScheduler : public Scheduler<FIFOScheduler> {
   public:
    explicit FIFOScheduler() : stop_(false) {}

    template <class F, class... Args>
    decltype(auto) add(F&& f, Args&&... args) {
        // deduce return type
        using return_type = std::result_of<F(Args...)>::type;

        // fetch task
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        // Ensure the task is valid
        if (!task) {
            throw std::runtime_error("Task is null");
        }

        std::future<return_type> res = task->get_future();

        // critical section
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            if (stop_) throw std::runtime_error("enqueue on stopped ThreadPool");
            tasks_.emplace([task] { (*task)(); });
        }

        // notify a wait thread
        condition_.notify_one();
        return res;
    }

    std::function<void()> get() {
        std::function<void()> task;

        // critical section
        {
            std::unique_lock<std::mutex> lock(this->queue_mutex_);

            condition_.wait(lock,
                            [this] { return this->stop_ || !this->tasks_.empty(); });

            // return if queue empty and task finished
            if (stop_ && tasks_.empty()) return nullptr;

            // otherwise execute the first element of queue
            task = std::move(tasks_.front());

            tasks_.pop();
        }

        return task;
    }

    void stop() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            stop_ = true;
        }
        condition_.notify_all();
    }

   private:
    // queue task, the type of queue elements are functions with void return type
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    bool stop_;
};

#endif