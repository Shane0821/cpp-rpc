#include "rpc_coro_mgr.h"

RpcCoroMgr::context RpcCoroMgr::PopCoroContext(RpcCoroMgr::coro_uid_type coro_uid) {
    if (auto iter = suspended_contexts_.find(coro_uid);
        iter != suspended_contexts_.end()) {
        auto ctx = iter->second;
        suspended_contexts_.erase(iter);
        return ctx;
    }
    LLOG_ERROR("coro_uid not found|coro_uid:%lu", coro_uid);
    return context{.handle = nullptr, .rsp = nullptr};
}

void RpcCoroMgr::HandleCoroTimeout() {
    llbc::sint64 now = llbc::LLBC_GetMilliseconds();
    while (!coroHeap_.empty()) {
        auto &top = coroHeap_.top();
        if (top.timeoutTime_ > now) {
            break;
        }
        auto ctx = PopCoroContext(top.session_id);
        if (ctx.handle) {
            ctx.handle.destroy();
        }
        coroHeap_.pop();
    }
}