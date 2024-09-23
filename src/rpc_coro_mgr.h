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

    // Add coro context to map and timeout heap.
    bool AddCoroContext(context ctx) noexcept;

    // Kill a coro by coro_uid
    void KillCoro(coro_uid_type coro_uid, const std::string &reason) noexcept;

    // Kill a coro by context
    void KillCoro(context &ctx, const std::string &reason) noexcept;

    /**
     * Pop coro context by coro_uid.
     * @note This method does not erase the context from timeout heap.
     */
    context PopCoroContext(coro_uid_type coro_uid) noexcept;

    // Handle coro timeout.
    void HandleCoroTimeout() noexcept;

    // Generate new coro uid.
    static coro_uid_type NewCoroUid() noexcept {
        return ++coro_uid_generator_ == 0UL ? ++coro_uid_generator_ : coro_uid_generator_;
    }

    static constexpr int CORO_TIME_OUT = 10000;  // coro timeout time, 10s

   protected:
    RpcCoroMgr() = default;

   private:
    std::unordered_map<coro_uid_type, context>
        suspended_contexts_;  // suspended coro contextss
    llbc::LLBC_Heap<context, std::vector<context>, contextCmp> coroHeap_;  // timeout heap

    static coro_uid_type coro_uid_generator_;  // coro_uid generator, which should
                                               // generate unique id without `0`.
};

#endif  // _RPC_CORO_MGR_H_