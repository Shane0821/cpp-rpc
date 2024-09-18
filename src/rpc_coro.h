#ifndef _RPC_CORO_H_
#define _RPC_CORO_H_

#include <coroutine>
#include <stdexcept>

class RpcCoro {
   public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type {
        int unique_id;

        // Called to get the return object of the coroutine
        auto get_return_object() noexcept {
            return RpcCoro{handle_type::from_promise(*this)};
        }

        // Define when the coroutine initially suspends
        auto initial_suspend() noexcept { return std::suspend_never{}; }

        // Define behavior at the end of the coroutine
        auto final_suspend() noexcept { return std::suspend_always{}; }

        // Handle exceptions
        void unhandled_exception() { std::terminate(); }

        // Return void from the coroutine
        void return_void() noexcept {}

        // Yield value
        auto yield_value() noexcept { return std::suspend_always{}; }
    };

    struct GetHandleAwaiter {
        bool await_ready() const noexcept { return false; }

        bool await_suspend(std::coroutine_handle<promise_type> handle) {
            handle_ = handle.address();
            return false;
        }

        void *await_resume() noexcept { return handle_; }

        void *handle_;
    };

    RpcCoro(RpcCoro const &) = delete;
    RpcCoro(RpcCoro &&rhs) noexcept : coro_handle(rhs.coro_handle) {
        rhs.coro_handle = nullptr;
    }
    ~RpcCoro() {
        if (coro_handle) coro_handle.destroy();
    }

    int get_id() const noexcept { return coro_handle.promise().unique_id; }
    void resume() noexcept { coro_handle.resume(); }
    void cancel() noexcept {
        if (coro_handle) {
            coro_handle.destroy();
            coro_handle = nullptr;
        }
    }

   private:
    /**
     * This is set to private, to make sure that:
     * 1. only the coroutine infrastructure (via get_return_object) can create instances
     * 2. the promise type is tightly coupled with the coroutine's return object
     */
    RpcCoro(handle_type h) noexcept : coro_handle(h) {}

    handle_type coro_handle;
};

#endif  // _RPC_CORO_H_