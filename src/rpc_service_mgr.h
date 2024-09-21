#ifndef _RPC_SERVICE_MGR_H_
#define _RPC_SERVICE_MGR_H_

#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <google/protobuf/stubs/common.h>
#include <llbc.h>
#include <singleton.h>

#include "rpc_channel.h"

class RpcController;
class RpcConnMgr;

class RpcServiceMgr : public Singleton<RpcServiceMgr> {
    friend class Singleton<RpcServiceMgr>;

   public:
    struct ServiceInfo {
        ::google::protobuf::Service *service = nullptr;
        const ::google::protobuf::MethodDescriptor *md = nullptr;
    };

    virtual ~RpcServiceMgr();

    int Init(RpcConnMgr *conn_mgr) noexcept;
    bool AddService(::google::protobuf::Service *service) noexcept;
    bool RegisterChannel(const char *ip, int32_t port) noexcept;

   protected:
    RpcServiceMgr() = default;

   private:
    // 处理 RPC 请求和返回包
    void HandleRpcReq(llbc::LLBC_Packet &packet);
    void HandleRpcRsp(llbc::LLBC_Packet &packet);

    // 处理 RPC 结束回调
    void OnRpcDone(
        RpcController *controller,
        std::pair<::google::protobuf::Message *, ::google::protobuf::Message *>);

   private:
    RpcConnMgr *conn_mgr_ = nullptr;

    std::vector<RpcChannel *> channels_;
    std::unordered_map<std::string, std::unordered_map<std::string, ServiceInfo>>
        service_methods_;
};  // RpcServiceMgr

#endif  // _RPC_SERVICE_MGR_H_