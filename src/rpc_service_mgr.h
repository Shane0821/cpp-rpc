#ifndef _RPC_SERVICE_MGR_H_
#define _RPC_SERVICE_MGR_H_

#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <google/protobuf/stubs/common.h>
#include <llbc.h>
#include <singleton.h>

#include "rpc_channel.h"
#include "rpc_registry.h"

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

    // add an user implemented service
    int AddService(::google::protobuf::Service *service) noexcept;

    // register rpc channel. if channel already exists, return it directly.
    RpcChannel *RegisterRpcChannel(const char *, int port) noexcept;

   protected:
    RpcServiceMgr() = default;

    // handle rpc request packet
    virtual void HandleRpcReq(llbc::LLBC_Packet &packet) noexcept;
    // handle rpc response packet
    virtual void HandleRpcRsp(llbc::LLBC_Packet &packet) noexcept;

   private:
    // called on rpc request done, send response back
    void OnRpcDone(
        RpcController *controller,
        std::pair<::google::protobuf::Message *, ::google::protobuf::Message *>) noexcept;

    RpcConnMgr *conn_mgr_ = nullptr;
    std::unique_ptr<RpcRegistry> registry_;
    std::unordered_map<std::string, std::unordered_map<std::string, ServiceInfo>>
        service_methods_;  // service_name -> method_name -> service_info
    std::unordered_map<std::string, RpcChannel *> channels_;  // ip:port -> channel
};  // RpcServiceMgr

#endif  // _RPC_SERVICE_MGR_H_