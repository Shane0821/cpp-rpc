#ifndef _RPC_CONTROLLER_H
#define _RPC_CONTROLLER_H

#include <google/protobuf/service.h>

#include "rpc_channel.h"

class RpcController : public ::google::protobuf::RpcController {
   public:
    RpcController() = default;
    ~RpcController() = default;
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

    void SetPkgHead(const RpcChannel::PkgHead& pkg_head) { pkg_head_ = pkg_head; }
    const RpcChannel::PkgHead& GetPkgHead() const { return pkg_head_; }

   private:
    bool isFailed_ = false;
    std::string errorText_;
    RpcChannel::PkgHead pkg_head_;
};

#endif  // _RPC_CONTROLLER_H
