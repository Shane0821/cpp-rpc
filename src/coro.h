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
        auto get_return_object() { return RpcCoro{handle_type::from_promise(*this)}; }

        // Define when the coroutine initially suspends
        auto initial_suspend() { return std::suspend_never{}; }

        // Define behavior at the end of the coroutine
        auto final_suspend() noexcept { return std::suspend_always{}; }

        // Handle exceptions
        void unhandled_exception() { std::terminate(); }

        // Return void from the coroutine
        void return_void() {}

        // Yield value
        auto yield_value() { return std::suspend_always{}; }
    };

    RpcCoro(RpcCoro const &) = delete;
    RpcCoro(RpcCoro &&rhs) : coro_handle(rhs.coro_handle) { rhs.coro_handle = nullptr; }

    ~RpcCoro() {
        if (coro_handle) coro_handle.destroy();
    }

    int get_id() const { return coro_handle.promise().unique_id; }
    void resume() { coro_handle.resume(); }
    void cancel() {
        if (coro_handle) {
            coro_handle.destroy();
            coro_handle = nullptr;
        }
    }

   private:
    RpcCoro(handle_type h) : coro_handle(h) {}

    handle_type coro_handle;
};

#endif  // _RPC_CORO_H_