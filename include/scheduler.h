#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <chrono>              // std::chrono
#include <condition_variable>  // std::condition_variable
#include <functional>          // std::function, std::bind
#include <future>              // std::future, std::packaged_task
#include <memory>              // std::make_shared
#include <mutex>               // std::mutex, std::unique_lock
#include <queue>               // std::queue
#include <set>                 // std::set
#include <stack>               // std::stack
#include <stdexcept>           // std::runtime_error
#include <thread>              // std::thread
#include <unordered_map>       // std::unordered_map
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

    bool execute() { return static_cast<T*>(this)->execute(); }

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
            if (stop_) throw std::runtime_error("add on stopped scheduler");
            tasks_.emplace([task] { (*task)(); });
        }

        // notify a wait thread
        condition_.notify_one();
        return res;
    }

    bool execute() {
        std::function<void()> task;

        // critical section
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);

            condition_.wait(lock,
                            [this] { return this->stop_ || !this->tasks_.empty(); });

            // return if queue empty and task finished
            if (stop_ && tasks_.empty()) return false;

            // otherwise execute the first element of queue
            task = std::move(tasks_.front());

            tasks_.pop();
        }

        task();
        return true;
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

class LIFOScheduler : public Scheduler<LIFOScheduler> {
   public:
    explicit LIFOScheduler() : stop_(false) {}

    template <class F, class... Args>
    decltype(auto) add(F&& f, Args&&... args) {
        using return_type = std::result_of_t<F(Args...)>;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        auto fut = task->get_future();

        {
            std::unique_lock<std::mutex> lock(stack_mutex_);
            if (stop_) throw std::runtime_error("add on stopped scheduler");
            tasks_.emplace([task] { (*task)(); });
        }

        condition_.notify_one();

        return fut;
    }

    bool execute() {
        std::function<void()> task;

        // critical section
        {
            std::unique_lock<std::mutex> lock(stack_mutex_);

            condition_.wait(lock,
                            [this] { return this->stop_ || !this->tasks_.empty(); });

            // return if queue empty and task finished
            if (stop_ && tasks_.empty()) return false;

            // otherwise execute the first element of queue
            task = std::move(tasks_.top());

            tasks_.pop();
        }

        task();
        return true;
    }

    void stop() {
        {
            std::unique_lock<std::mutex> lock(stack_mutex_);
            stop_ = true;
        }
        condition_.notify_all();
    }

   private:
    // queue task, the type of queue elements are functions with void return type
    std::stack<std::function<void()>> tasks_;
    std::mutex stack_mutex_;
    std::condition_variable condition_;
    bool stop_;
};

class TimerEventScheduler {
   public:
    explicit TimerEventScheduler() : stop_(false) {}

    template <class F, class... Args>
    decltype(auto) add(std::chrono::milliseconds delay, F&& f, Args&&... args) {
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
        const size_t id = ID_.fetch_add(1);
        TimerEvent event{id, [task] { (*task)(); },
                         std::chrono::system_clock::now() + delay};

        // critical section
        {
            std::unique_lock<std::mutex> lock(set_mutex_);
            if (stop_) throw std::runtime_error("add on stopped scheduler");
            auto [it, success] = tasks_.insert(std::move(event));
            if (!success) throw std::runtime_error("double add");
            id_it_map_[id] = it;
        }

        // notify a wait thread
        condition_.notify_one();
        return std::make_pair(id, std::move(res));
    }

    bool execute() {
        std::unique_lock<std::mutex> lock(set_mutex_);

        condition_.wait(lock, [this] { return this->stop_ || !this->tasks_.empty(); });

        // return if task is finished
        if (stop_ && tasks_.empty()) return false;

        // otherwise pop the first element of tasks
        auto task = std::move(*tasks_.begin());
        tasks_.erase(tasks_.begin());
        id_it_map_.erase(task.id_);

        lock.unlock();

        if (task.trigger_time_ > std::chrono::system_clock::now()) {
            std::this_thread::sleep_for(task.trigger_time_ -
                                        std::chrono::system_clock::now());
        }

        task.callback_();
        return true;
    }

    void cancel(size_t id) {
        std::unique_lock<std::mutex> lock(set_mutex_);
        if (!id_it_map_.count(id)) return;

        tasks_.erase(id_it_map_[id]);
        id_it_map_.erase(id);
    }

    void stop() {
        {
            std::unique_lock<std::mutex> lock(set_mutex_);
            stop_ = true;
        }
        condition_.notify_all();
    }

   private:
    struct TimerEvent {
        size_t id_;
        std::function<void()> callback_;
        std::chrono::time_point<std::chrono::system_clock> trigger_time_;

        TimerEvent(size_t id, std::function<void()> cb,
                   std::chrono::time_point<std::chrono::system_clock> t)
            : trigger_time_(t), callback_(std::move(cb)), id_(id) {}

        bool operator<(const TimerEvent& other) const {
            return trigger_time_ < other.trigger_time_;
        }
    };

    std::set<TimerEvent> tasks_;
    std::unordered_map<std::size_t, decltype(tasks_)::iterator> id_it_map_;
    std::atomic<size_t> ID_{0};
    std::mutex set_mutex_;
    std::condition_variable condition_;
    bool stop_;
};

#endif