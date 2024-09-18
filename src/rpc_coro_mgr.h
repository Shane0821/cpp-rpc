#ifndef _RPC_CORO_MGR_H_
#define _RPC_CORO_MGR_H_

#include <google/protobuf/message.h>
#include <llbc.h>

#include <coroutine>
#include <unordered_map>

#include "rpc_coro.h"
#include "rpc_macros.h"

class RpcCoroMgr : public Singleton<RpcCoroMgr> {
    friend class Singleton<RpcCoroMgr>;

   public:
    using coro_uid_type = std::uint64_t;

    struct context {
        int session_id = 0;
        llbc::sint64 timeoutTime_;
        std::coroutine_handle<RpcCoro::promise_type> handle = nullptr;
        ::google::protobuf::Message *rsp = nullptr;
    };

    virtual ~RpcCoroMgr() = default;

    int Init() { return 0; }

    bool UseCoro() const noexcept { return use_coro_; }
    void SetUseCoro(bool use_coro) noexcept { use_coro_ = use_coro; }

    coro_uid_type NewCoroUid() noexcept {
        COND_RET(!use_coro_, 0UL);
        return ++coro_uid_generator_ == 0UL ? ++coro_uid_generator_ : coro_uid_generator_;
    }

    bool AddCoroContext(coro_uid_type coro_uid, context ctx) noexcept {
        return suspended_contexts_.insert({coro_uid, ctx}).second;
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

    static constexpr int CORO_TIME_OUT = 10000;  // 10s

   protected:
    RpcCoroMgr() {}

   private:
    std::unordered_map<coro_uid_type, context> suspended_contexts_;
    // coro_uid generator, which should generate unique id without `0`.
    coro_uid_type coro_uid_generator_ = 0UL;
    bool use_coro_ = false;
};

// Used with the co_await keyword in coroutines.
struct RpcSaveContextAwaiter {
   public:
    RpcSaveContextAwaiter(RpcCoroMgr::coro_uid_type coro_uid, RpcCoroMgr::context context)
        : coro_uid_(coro_uid), context_(context) {}

    bool await_ready() const noexcept {
        // Always returns false, meaning the coroutine will always suspend when this
        // awaiter is used.
        return false;
    }

    // Called when the coroutine is suspended
    decltype(auto) await_suspend(std::coroutine_handle<RpcCoro::promise_type> handle) {
        LLOG_INFO("suspend coro|coro_uid:%lu|%p|rsp:%p", coro_uid_, handle.address(),
                  context_.rsp);
        context_.handle = handle;
        RpcCoroMgr::GetInst().AddCoroContext(coro_uid_, context_);
        // false: Immediate resumption after suspension.
        // true: Suspension until explicitly resumed by external logic.
        return true;
    }

    // Called when the coroutine is resumed.
    void await_resume() {}

   private:
    RpcCoroMgr::coro_uid_type coro_uid_ = 0UL;
    RpcCoroMgr::context context_;
};

#endif  // _RPC_CORO_MGR_H_