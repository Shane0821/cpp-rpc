#ifndef _RPC_CONTROLLER_H
#define _RPC_CONTROLLER_H

#include <google/protobuf/service.h>

#include "rpc_channel.h"

class RpcController : public ::google::protobuf::RpcController {
   public:
    RpcController() noexcept = default;
    ~RpcController() noexcept = default;

    virtual void Reset() {}
    virtual bool Failed() const { return isFailed_; };
    virtual std::string ErrorText() const { return errorText_; };
    virtual void StartCancel() {}
    virtual void SetFailed(const std::string& reason) {
        isFailed_ = true;
        errorText_ = reason;
    };
    virtual bool IsCanceled() const { return false; }
    virtual void NotifyOnCancel(::google::protobuf::Closure* /* callback */) {}

    void SetPkgHead(const RpcChannel::PkgHead& pkg_head) noexcept {
        pkg_head_ = pkg_head;
    }
    RpcChannel::PkgHead& GetPkgHead() noexcept { return pkg_head_; }

    void SetSessionId(int session_id) noexcept { session_id_ = session_id; }
    const int& GetSessionId() const noexcept { return session_id_; }

    void SetCoroHandle(void* handle) noexcept { coro_handle = handle; }
    void* GetCoroHandle() noexcept { return coro_handle; }

   private:
    bool isFailed_ = false;
    std::string errorText_;
    RpcChannel::PkgHead pkg_head_;
    int session_id_;
    void* coro_handle;
};

#endif  // _RPC_CONTROLLER_H
