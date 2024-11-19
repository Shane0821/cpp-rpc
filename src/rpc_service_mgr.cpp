#include "rpc_service_mgr.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/text_format.h>

#include "rpc_conn_mgr.h"
#include "rpc_controller.h"
#include "rpc_coro_mgr.h"
#include "rpc_macros.h"

int RpcServiceMgr::Init(RpcConnMgr *conn_mgr) noexcept {
    conn_mgr_ = conn_mgr;
    if (conn_mgr_) [[likely]] {
        conn_mgr_->Subscribe(RpcChannel::RpcOpCode::RpcReq,
                             llbc::LLBC_Delegate<void(llbc::LLBC_Packet &)>(
                                 this, &RpcServiceMgr::HandleRpcReq));
        conn_mgr_->Subscribe(RpcChannel::RpcOpCode::RpcRsp,
                             llbc::LLBC_Delegate<void(llbc::LLBC_Packet &)>(
                                 this, &RpcServiceMgr::HandleRpcRsp));
    }
    registry_ = std::make_unique<RpcRegistry>();
    return registry_->Connect("127.0.0.1:2181");
}

RpcServiceMgr::~RpcServiceMgr() {}

int RpcServiceMgr::AddService(::google::protobuf::Service *service) noexcept {
    const auto *service_desc = service->GetDescriptor();
    for (int i = 0; i < service_desc->method_count(); ++i) {
        auto *method_desc = service_desc->method(i);
        service_methods_[service_desc->name()][method_desc->name()] =
            ServiceInfo{service, method_desc};
        // register service to zookeeper
        LLOG_TRACE("RpcServiceMgr AddService: %s.%s, IP: %s",
                   service_desc->name().c_str(), method_desc->name().c_str(),
                   conn_mgr_->GetIP().c_str());
        auto ret = registry_->RegisterService(
            service_desc->name() + "." + method_desc->name(), conn_mgr_->GetIP());
        COND_RET(ret != LLBC_OK, ret);
    }
    return LLBC_OK;
}

RpcChannel *RpcServiceMgr::RegisterRpcChannel(const std::string &svc_md) noexcept {
    auto [ip, port] = registry_->GetRandomService(svc_md);
    COND_RET_ELOG(ip == "" || port == 0, nullptr,
                  "RegisterRpcChannel: service not found|svc_md:%s", svc_md.c_str());
    auto key = ip + ":" + std::to_string(port);
    if (auto it = channels_.find(key); it != channels_.end()) {
        return it->second;
    }
    auto *channel = conn_mgr_->CreateRpcChannel(ip.c_str(), port);
    if (channel)
        channels_[key] = channel;
    else
        LLOG_ERROR("RegisterRpcChannel: create channel failed|ip:%s|port:%d", ip.c_str(),
                   port);
    return channel;
}

void RpcServiceMgr::HandleRpcReq(llbc::LLBC_Packet &packet) noexcept {
    RpcChannel::PkgHead pkg_head;
    int ret = pkg_head.FromPacket(packet);
    COND_RET_ELOG(ret != 0, , "HandleRpcReq: pkg_head.FromPacket failed|ret:%d", ret);

    auto it = service_methods_.find(pkg_head.service_name);
    COND_RET_ELOG(it == service_methods_.end(), ,
                  "HandleRpcReq: service not found|service_name:%s",
                  pkg_head.service_name.c_str());
    auto iter = it->second.find(pkg_head.method_name);
    COND_RET_ELOG(iter == it->second.end(), ,
                  "HandleRpcReq: method not found|method_name:%s",
                  pkg_head.method_name.c_str());

    auto *service = iter->second.service;
    const auto *md = iter->second.md;

    // parse req
    auto *req = service->GetRequestPrototype(md).New();
    LLOG_TRACE("HandleRpcReq: packet: %s", packet.ToString().c_str());
    ret = packet.Read(*req);
    COND_RET_ELOG(ret != LLBC_OK, delete req,
                  "HandleRpcReq: read req failed|ret:%d|reason:%s", ret,
                  llbc::LLBC_FormatLastError());
    // create rsp
    auto *rsp = service->GetResponsePrototype(md).New();

    auto controller = new RpcController(true);
    // TODO: auto controller = objPool.Get<RpcController>();
    controller->SetSessionID(packet.GetSessionId());
    controller->SetPkgHead(pkg_head);

    // create call back on rpc done
    // service methods should call done->run on rpc completion
    auto done = ::google::protobuf::NewCallback(this, &RpcServiceMgr::OnRpcDone,
                                                controller, {req, rsp});
    service->CallMethod(md, controller, req, rsp, done);
}

void RpcServiceMgr::HandleRpcRsp(llbc::LLBC_Packet &packet) noexcept {
    LLOG_TRACE("HandleRpcRsp: packet: %s", packet.ToString().c_str());
    RpcChannel::PkgHead pkg_head;
    int ret = pkg_head.FromPacket(packet);
    // this should not happen
    COND_RET_ELOG(ret != LLBC_OK, , "HandleRpcRsp: pkg_head.FromPacket failed|ret:%d",
                  ret);
    LLOG_DEBUG("HandleRpcRsp: pkg_head info|%s", pkg_head.ToString().c_str());

    auto coro_uid = static_cast<RpcCoroMgr::coro_uid_type>(pkg_head.seq);
    auto ctx = RpcCoroMgr::GetInst().PopCoroContext(coro_uid);

    // the coro context is already removed
    // the coro is already killed
    COND_RET_WLOG(ctx.handle == nullptr || ctx.controller == nullptr, ,
                  "HandleRpcRsp: coro context not found (possibly due to "
                  "timeout)|seq_id:%lu|service_name:%s|method_name:%s|",
                  coro_uid, pkg_head.service_name.c_str(), pkg_head.method_name.c_str());

    COND_RET_ELOG(ctx.controller->UseCoro() == false, ,
                  "HandleRpcRsp: controller is blocking controller");

    // failed due to other reasons
    if (packet.GetStatus() != LLBC_OK) {
        ctx.controller->SetFailed("rpc failed");
        ctx.handle.resume();
        LLOG_INFO("HandleRpcRsp: coro is done|%u", ctx.handle.done());
        return;
    }
    if (ctx.controller->Failed()) {
        ctx.handle.resume();
        LLOG_INFO("HandleRpcRsp: coro is done|%u", ctx.handle.done());
        return;
    }

    // no response, just resume the coro
    if (!ctx.rsp) {
        LLOG_INFO("HandleRpcRsp: ctx doesn't have rsp");
        ctx.handle.resume();
        LLOG_INFO("HandleRpcRsp: coro is done|%u", ctx.handle.done());
        return;
    }

    ret = packet.Read(*ctx.rsp);
    COND_RET_ELOG(ret != LLBC_OK, RpcCoroMgr::GetInst().KillCoro(ctx, "read rsp failed"),
                  "HandleRpcRsp: read rsp failed|ret:%d", ret);
    LLOG_INFO("HandleRpcRsp: received rsp|address:%p|info:%s|sesson_id:%d", ctx.rsp,
              ctx.rsp->DebugString().c_str(), packet.GetSessionId());

    ctx.handle.resume();
    LLOG_INFO("HandleRpcRsp: coro is done|%u", ctx.handle.done());
}

void RpcServiceMgr::OnRpcDone(
    RpcController *controller,
    std::pair<::google::protobuf::Message *, ::google::protobuf::Message *>
        req_rsp) noexcept {
    COND_EXP_ELOG(controller == nullptr, , "OnRpcDone: controller is nullptr");

    auto &[req, rsp] = req_rsp;

    llbc::LLBC_Packet *packet = llbc::LLBC_GetObjectFromSafetyPool<llbc::LLBC_Packet>();

    auto cleanUp = [&]() {
        delete req;
        delete rsp;
        delete controller;
    };

    COND_RET_ELOG(
        !packet, cleanUp(),
        "OnRpcDone: alloc packet from obj pool failed|pkg_head: %s|req: %s|rsp: %s",
        controller->GetPkgHead().ToString().c_str(),
        req ? req->ShortDebugString().c_str() : "",
        rsp ? rsp->ShortDebugString().c_str() : "");

    packet->SetOpcode(RpcChannel::RpcOpCode::RpcRsp);
    packet->SetSessionId(controller->GetSessionID());

    int ret = controller->GetPkgHead().ToPacket(*packet);
    COND_RET_ELOG(ret != 0, cleanUp(), "OnRpcDone: pkg_head.ToPacket failed|ret:%d", ret);

    if (controller->Failed()) {
        packet->SetStatus(LLBC_FAILED);
    }

    ret = packet->Write(*rsp);
    COND_RET_ELOG(ret != 0, cleanUp(), "OnRpcDone: packet.Write failed|ret:%d", ret);

    LLOG_TRACE("OnRpcDone: packet: %s", packet->ToString().c_str());

    conn_mgr_->SendPacket(packet);
    cleanUp();
}
