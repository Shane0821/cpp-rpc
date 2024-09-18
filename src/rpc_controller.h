#ifndef _RPC_CONTROLLER_H
#define _RPC_CONTROLLER_H

#include <google/protobuf/service.h>

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

   private:
    bool isFailed_ = false;
    std::string errorText_;
};

#endif  // _RPC_CONTROLLER_H
