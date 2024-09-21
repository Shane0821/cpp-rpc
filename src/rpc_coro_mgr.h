#ifndef _RPC_CORO_MGR_H_
#define _RPC_CORO_MGR_H_

#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>
#include <llbc.h>

#include <coroutine>
#include <unordered_map>

#include "rpc_controller.h"
#include "rpc_coro.h"
#include "rpc_macros.h"

class RpcCoroMgr : public Singleton<RpcCoroMgr> {
    friend class Singleton<RpcCoroMgr>;

   public:
    using coro_uid_type = std::uint64_t;

    struct context {
        int session_id = 0;
        coro_uid_type coro_uid = 0UL;
        llbc::sint64 timeout_time;
        std::coroutine_handle<RpcCoro::promise_type> handle = nullptr;
        ::google::protobuf::Message *rsp = nullptr;
        RpcController *controller = nullptr;
    };

    struct contextCmp {
        bool operator()(context &a, context &b) {
            return a.timeout_time < b.timeout_time;
        }
    };

    virtual ~RpcCoroMgr() = default;

    int Init() { return 0; }

    bool UseCoro() const noexcept { return use_coro_; }
    void SetUseCoro(bool use_coro) noexcept { use_coro_ = use_coro; }

    coro_uid_type NewCoroUid() noexcept {
        COND_RET(!use_coro_, 0UL);
        return ++coro_uid_generator_ == 0UL ? ++coro_uid_generator_ : coro_uid_generator_;
    }

    bool AddCoroContext(context ctx) noexcept {
        return suspended_contexts_.insert({ctx.coro_uid, ctx}).second;
    }

    context PopCoroContext(coro_uid_type coro_uid) {
        if (auto iter = suspended_contexts_.find(coro_uid);
            iter != suspended_contexts_.end()) {
            auto ctx = iter->second;
            suspended_contexts_.erase(iter);
            return ctx;
        }
        LLOG_ERROR("coro_uid not found|coro_uid:%lu", coro_uid);
        return context{.handle = nullptr, .rsp = nullptr};
    }

    void HandleCoroTimeout() {
        llbc::sint64 now = llbc::LLBC_GetMilliseconds();
        while (!coroHeap_.empty()) {
            auto &top = coroHeap_.top();
            if (top.timeout_time > now) {
                break;
            }
            coroHeap_.pop();
            auto ctx = PopCoroContext(top.coro_uid);
            if (ctx.handle == nullptr) {
                continue;
            }
            ctx.controller->SetFailed("coro timeout");
            google::protobuf::TextFormat::ParseFromString("coro timeout", ctx.rsp);
            ctx.handle.resume();
        }
    }

    static constexpr int CORO_TIME_OUT = 10000;  // 10s

   protected:
    RpcCoroMgr() = default;

   private:
    std::unordered_map<coro_uid_type, context> suspended_contexts_;
    llbc::LLBC_Heap<context, std::vector<context>, contextCmp> coroHeap_;
    // coro_uid generator, which should generate unique id without `0`.
    coro_uid_type coro_uid_generator_ = 0UL;
    bool use_coro_ = false;
};

// Used with the co_await keyword in coroutines.
struct RpcSaveContextAwaiter {
   public:
    RpcSaveContextAwaiter(RpcCoroMgr::context context) : context_(context) {}

    bool await_ready() const noexcept {
        // Always returns false, meaning the coroutine will always suspend when this
        // awaiter is used.
        return false;
    }

    // Called when the coroutine is suspended
    decltype(auto) await_suspend(std::coroutine_handle<RpcCoro::promise_type> handle) {
        LLOG_INFO("suspend coro|coro_uid:%lu|%p|rsp:%p", context_.coro_uid,
                  handle.address(), context_.rsp);
        context_.handle = handle;
        RpcCoroMgr::GetInst().AddCoroContext(context_);
        // false: Immediate resumption after suspension.
        // true: Suspension until explicitly resumed by external logic.
        return true;
    }

    // Called when the coroutine is resumed.
    void await_resume() {}

   private:
    RpcCoroMgr::context context_;
};

#endif  // _RPC_CORO_MGR_H_