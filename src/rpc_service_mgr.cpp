#include "rpc_service_mgr.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>

#include "rpc_conn_mgr.h"
#include "rpc_controller.h"
#include "rpc_coro_mgr.h"
#include "rpc_macros.h"

int RpcServiceMgr::Init(RpcConnMgr *conn_mgr) noexcept {
    COND_RET_ELOG(conn_mgr_ != nullptr, LLBC_FAILED,
                  "RpcConnMgr has already been registered|address:%p", conn_mgr_);
    conn_mgr_ = conn_mgr;
    if (conn_mgr_) [[likely]] {
        conn_mgr_->Subscribe(RpcChannel::RpcOpCode::RpcReq,
                             llbc::LLBC_Delegate<void(llbc::LLBC_Packet &)>(
                                 this, &RpcServiceMgr::HandleRpcReq));
        conn_mgr_->Subscribe(RpcChannel::RpcOpCode::RpcRsp,
                             llbc::LLBC_Delegate<void(llbc::LLBC_Packet &)>(
                                 this, &RpcServiceMgr::HandleRpcRsp));
    }
    return LLBC_OK;
}

RpcServiceMgr::~RpcServiceMgr() {
    if (conn_mgr_) [[likely]] {
        conn_mgr_->Unsubscribe(RpcChannel::RpcOpCode::RpcReq);
        conn_mgr_->Unsubscribe(RpcChannel::RpcOpCode::RpcRsp);
    }
}

bool RpcServiceMgr::AddService(::google::protobuf::Service *service) noexcept {
    const auto *service_desc = service->GetDescriptor();
    for (int i = 0; i < service_desc->method_count(); ++i) {
        auto *method_desc = service_desc->method(i);
        service_methods_[service_desc->name()][method_desc->name()] =
            ServiceInfo{service, method_desc};
    }
    return true;
}

bool RpcServiceMgr::RegisterChannel(const char *ip, int32_t port) noexcept {
    auto *channel = conn_mgr_->CreateRpcChannel(ip, port);
    COND_RET_ELOG(!channel, false, "create rpc channel failed");
    channels_.emplace_back(channel);
    return true;
}

void RpcServiceMgr::HandleRpcReq(llbc::LLBC_Packet &packet) {
    RpcChannel::PkgHead pkg_head;
    int ret = pkg_head.FromPacket(packet);
    COND_RET_ELOG(ret != 0, , "pkg_head.FromPacket failed|ret:%d", ret);
    session_id_ = packet.GetSessionId();

    // auto iter = service_methods_.find(pkg_head.cmd);
    // COND_RET_ELOG(iter == service_methods_.end(), ,
    //               "service and method not found|cmd:%08X", pkg_head.cmd);
    // auto *service = iter->second.service;
    // const auto *method = iter->second.md;

    // // 解析 req & 创建 rsp
    // auto *req = service->GetRequestPrototype(method).New();
    // LLOG_TRACE("packet: %s", packet.ToString().c_str());
    // ret = packet.Read(*req);
    // COND_RET_ELOG(ret != LLBC_OK, , "read req failed|ret:%d|reason: %s", ret,
    //               llbc::LLBC_FormatLastError());
    // auto *rsp = service->GetResponsePrototype(method).New();

    // auto controller = new RpcController();
    // // TODO: auto controller = objPool.Get<RpcController>();
    // controller->SetPkgHead(pkg_head);
    // // create call back on rpc done
    // auto done =
    //     ::google::protobuf::NewCallback(this, &RpcServiceMgr::OnRpcDone, controller,
    //     rsp);
    // service->CallMethod(method, controller, req, rsp, done);
}

void RpcServiceMgr::HandleRpcRsp(llbc::LLBC_Packet &packet) {
    RpcChannel::PkgHead pkg_head;
    int ret = pkg_head.FromPacket(packet);
    COND_RET_ELOG(ret != LLBC_OK, , "pkg_head.FromPacket failed|ret:%d", ret);
    LLOG_DEBUG("pkg_head info|%s", pkg_head.ToString().c_str());

    auto coro_uid = static_cast<RpcCoroMgr::coro_uid_type>(pkg_head.seq);
    auto ctx = RpcCoroMgr::GetInst().PopCoroContext(coro_uid);
    COND_RET_ELOG(ctx.handle == nullptr || ctx.rsp == nullptr, ,
                  "coro context not found|seq_id:%lu|service_name:%s|method_name:%s|",
                  coro_uid, pkg_head.service_name.c_str(), pkg_head.method_name.c_str());
    // 解析 rsp
    ret = packet.Read(*ctx.rsp);
    COND_RET_ELOG(ret != LLBC_OK, , "read rsp failed|ret:%d", ret);
    session_id_ = ctx.session_id;
    LLOG_INFO("received rsp|address:%p|info: %s|sesson_id:%d", ctx.rsp,
              ctx.rsp->DebugString().c_str(), session_id_);

    ctx.handle.resume();
}

void RpcServiceMgr::OnRpcDone(RpcController *controller,
                              ::google::protobuf::Message *rsp) {
    llbc::LLBC_Packet *packet =
        llbc::LLBC_ThreadSpecObjPool::GetSafeObjPool()->Acquire<llbc::LLBC_Packet>();

    packet->SetOpcode(RpcChannel::RpcOpCode::RpcRsp);
    packet->SetSessionId(session_id_);

    int ret = controller->GetPkgHead().ToPacket(*packet);
    COND_RET_ELOG(ret != 0, , "pkg_head.ToPacket failed|ret:%d", ret);

    ret = packet->Write(*rsp);
    COND_RET_ELOG(ret != 0, , "packet.Write failed|ret:%d", ret);

    conn_mgr_->SendPacket(*packet);
}
