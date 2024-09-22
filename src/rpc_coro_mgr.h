#ifndef _RPC_CORO_MGR_H_
#define _RPC_CORO_MGR_H_

#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>
#include <llbc.h>
#include <singleton.h>

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

    static coro_uid_type NewCoroUid() noexcept {
        return ++coro_uid_generator_ == 0UL ? ++coro_uid_generator_ : coro_uid_generator_;
    }

    bool AddCoroContext(context ctx) noexcept {
        coroHeap_.push(ctx);
        return suspended_contexts_.insert({ctx.coro_uid, ctx}).second;
    }

    void KillCoro(coro_uid_type coro_uid, const std::string &reason) noexcept {
        if (auto iter = suspended_contexts_.find(coro_uid);
            iter != suspended_contexts_.end()) {
            auto ctx = iter->second;
            ctx.controller->SetFailed(reason);
            ctx.handle.resume();
            suspended_contexts_.erase(iter);
        }
    }

    void KillCoro(context &ctx, const std::string &reason) noexcept {
        ctx.controller->SetFailed(reason);
        ctx.handle.resume();
        suspended_contexts_.erase(ctx.coro_uid);
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
            auto top = coroHeap_.top();
            if (top.timeout_time > now) {
                break;
            }
            coroHeap_.pop();
            // already resumed
            if (suspended_contexts_.find(top.coro_uid) == suspended_contexts_.end()) {
                continue;
            }
            KillCoro(top, "coro timeout");
        }
    }

    static constexpr int CORO_TIME_OUT = 10000;  // 10s

   protected:
    RpcCoroMgr() = default;

   private:
    std::unordered_map<coro_uid_type, context> suspended_contexts_;
    llbc::LLBC_Heap<context, std::vector<context>, contextCmp> coroHeap_;
    // coro_uid generator, which should generate unique id without `0`.
    static coro_uid_type coro_uid_generator_;
    bool use_coro_ = false;
};

#endif  // _RPC_CORO_MGR_H_