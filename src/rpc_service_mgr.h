#ifndef _RPC_SERVICE_MGR_H_
#define _RPC_SERVICE_MGR_H_

#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <google/protobuf/stubs/common.h>
#include <llbc.h>
#include <singleton.h>

#include "rpc_channel.h"

class ConnMgr;

class RpcServiceMgr : public Singleton<RpcServiceMgr> {
    friend class Singleton<RpcServiceMgr>;

   public:
    // using CoMethodFunc =
    //     std::function<mt::Task<int>>(const ::google::protobuf::MethodDescriptor
    //     *method,
    //                                  ::google::protobuf::RpcController *controller,
    //                                  const ::google::protobuf::Message &request,
    //                                  ::google::protobuf::Message &response,
    //                                  ::google::protobuf::Closure *done) > ;
    struct ServiceInfo {
        ::google::protobuf::Service *service = nullptr;
        const ::google::protobuf::MethodDescriptor *method = nullptr;
        // CoMethodFunc co_func = nullptr;
    };

    virtual ~RpcServiceMgr();

    int Init(ConnMgr *conn_mgr);
    bool AddService(::google::protobuf::Service *service);
    bool RegisterChannel(const char *ip, int32_t port);

    void Rpc(std::uint32_t cmd, std::uint64_t uid, const ::google::protobuf::Message &req,
             ::google::protobuf::Message *rsp = nullptr, std::uint32_t timeout = 0U);

   protected:
    RpcServiceMgr() = default;

   private:
    // 处理 RPC 请求和返回包
    void HandleRpcReq(llbc::LLBC_Packet &packet);
    void HandleRpcRsp(llbc::LLBC_Packet &packet);

    // 处理 RPC 结束回调
    void OnRpcDone(const RpcChannel::PkgHead &pkg_head,
                   const ::google::protobuf::Message &rsp);

   private:
    ConnMgr *conn_mgr_ = nullptr;
    int session_id_;

    std::vector<RpcChannel *> channels_;
    std::unordered_map<std::uint32_t, ServiceInfo> service_methods_;
};  // RpcServiceMgr

#endif  // _RPC_SERVICE_MGR_H_