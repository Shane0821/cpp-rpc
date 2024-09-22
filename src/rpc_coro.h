#ifndef _RPC_CORO_H_
#define _RPC_CORO_H_

#include <llbc.h>

#include <coroutine>
#include <stdexcept>

class RpcCoro {
   public:
    struct promise_type;

    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type {
        auto get_return_object() noexcept {
            return RpcCoro{handle_type::from_promise(*this)};
        }
        auto initial_suspend() noexcept { return std::suspend_never{}; }
        auto final_suspend() noexcept {
            // std::suspend_always: The coroutine suspends at the end, requiring explicit
            // resumption by the caller.
            // std::suspend_never: The coroutine does not suspend at the end, allowing for
            // immediate cleanup and continuation without needing to resume.
            return std::suspend_never{};
        }
        void unhandled_exception() noexcept { std::terminate(); }
        void return_void() noexcept {}
        auto yield_value() noexcept { return std::suspend_always{}; }
    };

    explicit RpcCoro(handle_type h) : coro_handle_(h) {}
    RpcCoro(RpcCoro const&) = delete;
    RpcCoro(RpcCoro&& rhs) : coro_handle_(rhs.coro_handle_) {
        rhs.coro_handle_ = nullptr;
    }
    RpcCoro& operator=(RpcCoro&& other) noexcept {
        if (this != &other) {
            if (coro_handle_) coro_handle_.destroy();
            coro_handle_ = other.coro_handle_;
            other.coro_handle_ = nullptr;
        }
        return *this;
    }

    void resume() { coro_handle_.resume(); }
    auto yield() { return coro_handle_.promise().yield_value(); }
    void cancel() {
        coro_handle_.destroy();
        coro_handle_ = nullptr;
    }

   private:
    handle_type coro_handle_;
};

struct GetHandleAwaiter {
    bool await_ready() const noexcept { return false; }
    bool await_suspend(std::coroutine_handle<RpcCoro::promise_type> handle) {
        handle_ = handle.address();
        return false;  // Immediate resumption after suspension.
    }

    void* await_resume() noexcept { return handle_; }

    void* handle_;
};

#endif  // _RPC_CORO_H_