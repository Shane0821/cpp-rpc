#include "rpc_coro_mgr.h"

RpcCoroMgr::coro_uid_type RpcCoroMgr::coro_uid_generator_ = 0UL;

bool RpcCoroMgr::AddCoroContext(context ctx) noexcept {
    coroHeap_.Insert(ctx);
    return suspended_contexts_.insert({ctx.coro_uid, ctx}).second;
}

void RpcCoroMgr::KillCoro(coro_uid_type coro_uid, const std::string &reason) noexcept {
    if (auto iter = suspended_contexts_.find(coro_uid);
        iter != suspended_contexts_.end()) {
        auto ctx = iter->second;
        KillCoro(ctx, reason);
    }
}

void RpcCoroMgr::KillCoro(context &ctx, const std::string &reason) noexcept {
    ctx.controller->SetFailed(reason);
    ctx.handle.resume();
    suspended_contexts_.erase(ctx.coro_uid);
}

void RpcCoroMgr::HandleCoroTimeout() noexcept {
    llbc::sint64 now = llbc::LLBC_GetMilliSeconds();
    while (!coroHeap_.IsEmpty()) {
        auto top = coroHeap_.Top();
        if (top.timeout_time > now) {
            break;
        }
        coroHeap_.DeleteTop();
        // already resumed
        if (suspended_contexts_.find(top.coro_uid) == suspended_contexts_.end()) {
            continue;
        }
        KillCoro(top, "coro timeout");
    }
}

RpcCoroMgr::context RpcCoroMgr::PopCoroContext(coro_uid_type coro_uid) noexcept {
    if (auto iter = suspended_contexts_.find(coro_uid);
        iter != suspended_contexts_.end()) {
        auto ctx = iter->second;
        suspended_contexts_.erase(iter);
        return ctx;
    }
    LLOG_ERROR("coro_uid not found|coro_uid:%lu", coro_uid);
    return context{.handle = nullptr, .rsp = nullptr};
}