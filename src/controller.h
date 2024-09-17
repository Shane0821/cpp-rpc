#ifndef _RPC_CONTROLLER_H
#define _RPC_CONTROLLER_H

#include <google/protobuf/service.h>
#include <singleton.h>

class RpcController : public ::google::protobuf::RpcController,
                      public Singleton<RpcController> {
    friend class Singleton<RpcController>;

   public:
    ~RpcController() = default;
    virtual void Reset() {}
    virtual bool Failed() const { return false; }
    virtual std::string ErrorText() const { return ""; }
    virtual void StartCancel() {}
    virtual void SetFailed(const std::string& /* reason */) {}
    virtual bool IsCanceled() const { return false; }
    virtual void NotifyOnCancel(::google::protobuf::Closure* /* callback */) {}

   protected:
    RpcController() = default;
    RpcController(const RpcController&) = delete;
    RpcController& operator=(const RpcController&) = delete;
};

#endif  // _RPC_CONTROLLER_H
