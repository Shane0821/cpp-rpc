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

        auto get_return_object() { return RpcCoro{handle_type::from_promise(*this)}; }

        auto initial_suspend() { return std::suspend_never{}; }
        auto final_suspend() noexcept { return std::suspend_always{}; }
        void unhandled_exception() { std::terminate(); }
        void return_void() {}
        auto yield_value() { return std::suspend_always{}; }
    };

    RpcCoro(RpcCoro const &) = delete;
    RpcCoro(RpcCoro &&rhs) : coro_handle(rhs.coro_handle) { rhs.coro_handle = nullptr; }

    ~RpcCoro() {}

    int get_id() const { return coro_handle.promise().unique_id; }
    void resume() { coro_handle.resume(); }
    auto yield() { return coro_handle.promise().yield_value(); }
    void cancel() {
        coro_handle.destroy();
        coro_handle = nullptr;
    }

   private:
    RpcCoro(handle_type h) : coro_handle(h) {}

    handle_type coro_handle;
};

#endif  // _RPC_CORO_H_