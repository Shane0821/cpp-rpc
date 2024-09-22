#include "rpc_conn_mgr.h"

#include "rpc_channel.h"
#include "rpc_macros.h"

RpcConnMgr::~RpcConnMgr() noexcept {
    delete comp_;
    if (svc_) {
        svc_->Stop();
    }
}

int RpcConnMgr::Init() noexcept {
    if (svc_) {
        LLOG_ERROR("Service already started");
        return LLBC_FAILED;
    }
    // Create service
    svc_ = llbc::LLBC_Service::Create("Svc");
    comp_ = new RpcConnComp;
    svc_->AddComponent(comp_);
    svc_->SetDriveMode(llbc::LLBC_ServiceDriveMode::ExternalDrive);
    svc_->SetFPS(1000);
    svc_->Subscribe(RpcChannel::RpcOpCode::RpcReq, comp_, &RpcConnComp::OnRecvPacket);
    svc_->Subscribe(RpcChannel::RpcOpCode::RpcRsp, comp_, &RpcConnComp::OnRecvPacket);
    svc_->SuppressCoderNotFoundWarning();
    auto ret = svc_->Start(4);
    LLOG_TRACE("Service start, ret: %d", ret);
    return ret;
}

int RpcConnMgr::StartRpcService(const char *ip, int port) noexcept {
    if (is_server_) {
        LLOG_ERROR("Service already started");
        return LLBC_FAILED;
    }
    LLOG_TRACE("RpcConnMgr StartRpcService");
    LLOG_TRACE("Server will listen on %s:%d", ip, port);
    server_sessionId_ = svc_->Listen(ip, port);
    COND_RET_ELOG(server_sessionId_ == 0, LLBC_FAILED,
                  "Create session failed, reason: %s", llbc::LLBC_FormatLastError())
    is_server_ = true;
    return LLBC_OK;
}

RpcChannel *RpcConnMgr::CreateRpcChannel(const char *ip, int port) {
    LLOG_TRACE("CreateRpcChannel");

    // default timeout is -1, which means no timeout
    auto sessionId = svc_->Connect(ip, port);
    COND_RET_ELOG(sessionId == 0, nullptr, "Create session failed, reason: %s",
                  llbc::LLBC_FormatLastError());

    return new RpcChannel(this, sessionId);
}

int RpcConnMgr::CloseSession(int sessionId) {
    LLOG_TRACE("CloseSession, %d", sessionId);
    return svc_->RemoveSession(sessionId);
}

void RpcConnMgr::Tick() noexcept {
    svc_->OnSvc(true);
    llbc::LLBC_Packet *packet =
        llbc::LLBC_ThreadSpecObjPool::GetSafeObjPool()->Acquire<llbc::LLBC_Packet>();
    while (RecvPacket(*packet) == LLBC_OK) {
        LLOG_TRACE("Tick");
        auto it = packet_delegs_.find(packet->GetOpcode());
        if (it == packet_delegs_.end())
            LLOG_ERROR("Recv Untapped opcode:%d", packet->GetOpcode());
        else
            (it->second)(*packet);  // handle rep or handle rsp
    }
}

int RpcConnMgr::Subscribe(int cmdId,
                          const llbc::LLBC_Delegate<void(llbc::LLBC_Packet &)> &deleg) {
    auto pair = packet_delegs_.emplace(cmdId, deleg);
    COND_RET(!pair.second, LLBC_FAILED);
    return LLBC_OK;
}

void RpcConnMgr::Unsubscribe(int cmdId) { packet_delegs_.erase(cmdId); }