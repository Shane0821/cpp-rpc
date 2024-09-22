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

    bool AddCoroContext(context ctx) noexcept;

    void KillCoro(coro_uid_type coro_uid, const std::string &reason) noexcept;

    void KillCoro(context &ctx, const std::string &reason) noexcept;

    context PopCoroContext(coro_uid_type coro_uid) noexcept;

    void HandleCoroTimeout();

    static coro_uid_type NewCoroUid() noexcept {
        return ++coro_uid_generator_ == 0UL ? ++coro_uid_generator_ : coro_uid_generator_;
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