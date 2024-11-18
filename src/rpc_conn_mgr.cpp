#include "rpc_conn_mgr.h"

#include "rpc_channel.h"
#include "rpc_macros.h"

RpcConnMgr::~RpcConnMgr() noexcept {
    if (svc_) {
        svc_->Stop();
        svc_ = nullptr;
    }
}

int RpcConnMgr::Init() noexcept {
    if (svc_) {
        Destroy();
    }
    LLOG_TRACE("RpcConnMgr Init");
    // Create service
    svc_ = llbc::LLBC_Service::Create("Svc");  // newed
    if (!svc_) {
        LLOG_ERROR("Create LLBC service failed");
        return LLBC_FAILED;
    }
    comp_ = new RpcConnComp;
    int ret = svc_->AddComponent(comp_);
    COND_RET_ELOG(ret != LLBC_OK, LLBC_FAILED, "AddComponent failed, ret: %d", ret);

    ret = svc_->SetFPS(1000);
    COND_RET_ELOG(ret != LLBC_OK, LLBC_FAILED, "SetFPS failed, ret: %d", ret);

    ret =
        svc_->Subscribe(RpcChannel::RpcOpCode::RpcReq, comp_, &RpcConnComp::OnRecvPacket);
    COND_RET_ELOG(ret != LLBC_OK, LLBC_FAILED, "Subscribe RpcReq failed, ret: %d", ret);
    ret =
        svc_->Subscribe(RpcChannel::RpcOpCode::RpcRsp, comp_, &RpcConnComp::OnRecvPacket);
    COND_RET_ELOG(ret != LLBC_OK, LLBC_FAILED, "Subscribe RpcRsp failed, ret: %d", ret);

    ret = svc_->SuppressCoderNotFoundWarning();
    COND_RET_ELOG(ret != LLBC_OK, LLBC_FAILED,
                  "SuppressCoderNotFoundWarning failed, ret: %d", ret);

    ret = svc_->Start(4);
    COND_RET_ELOG(ret != LLBC_OK, LLBC_FAILED, "Start service failed, ret: %d", ret);
    return LLBC_OK;
}

void RpcConnMgr::Destroy() noexcept {
    if (svc_) {
        svc_->Stop();
        LLOG_TRACE("RpcConnMgr Svc Stopped");
        // delete svc_;  // components will be deleted by service
        svc_ = nullptr;
    }
    LLOG_TRACE("RpcConnMgr Destroyed");
}

int RpcConnMgr::StartRpcService(const char *ip, int port) noexcept {
    if (is_server_) {
        LLOG_ERROR("Service already started");
        return LLBC_FAILED;
    }
    LLOG_TRACE("RpcConnMgr StartRpcService");
    LLOG_TRACE("Server will listen on %s:%d", ip, port);
    server_sessionID_ = svc_->Listen(ip, port);
    COND_RET_ELOG(server_sessionID_ == 0, LLBC_FAILED,
                  "Create session failed, reason: %s", llbc::LLBC_FormatLastError())
    is_server_ = true;
    ip_ = std::string(ip) + "/" + std::to_string(port);
    return LLBC_OK;
}

RpcChannel *RpcConnMgr::CreateRpcChannel(const char *ip, int port) {
    LLOG_TRACE("CreateRpcChannel");

    // default timeout is -1, which means no timeout
    auto sessionID = svc_->Connect(ip, port);
    COND_RET_ELOG(sessionID == 0, nullptr, "Create session failed, reason: %s",
                  llbc::LLBC_FormatLastError());

    return new RpcChannel(this, sessionID);
}

int RpcConnMgr::CloseSession(int sessionID) {
    LLOG_TRACE("CloseSession: %d", sessionID);
    return svc_->RemoveSession(sessionID);
}

void RpcConnMgr::Tick() noexcept {
    llbc::LLBC_Packet *packet = nullptr;
    while (RecvPacket(packet) == LLBC_OK) {
        LLOG_TRACE("Tick: RecvPacket");
        auto it = packet_delegs_.find(packet->GetOpcode());
        if (it == packet_delegs_.end())
            LLOG_ERROR("Recv Untapped opcode:%d", packet->GetOpcode());
        else {
            (it->second)(*packet);  // handle rep or handle rsp
            LLBC_Recycle(packet);
        }
    }
}

int RpcConnMgr::Subscribe(int cmdID,
                          const llbc::LLBC_Delegate<void(llbc::LLBC_Packet &)> &deleg) {
    auto pair = packet_delegs_.emplace(cmdID, deleg);
    COND_RET(!pair.second, LLBC_FAILED);
    return LLBC_OK;
}

void RpcConnMgr::Unsubscribe(int cmdID) {
    if (packet_delegs_.find(cmdID) != packet_delegs_.end()) packet_delegs_.erase(cmdID);
}

int RpcConnMgr::BlockingRecvPacket(llbc::LLBC_Packet *&recvPacket) {
    int count = 0;
    while (RecvPacket(recvPacket) != LLBC_OK && count < RECEIVE_TIME_OUT) {
        llbc::LLBC_Sleep(1);
        count++;
    }
    if (count != RECEIVE_TIME_OUT) return LLBC_OK;
    return LLBC_FAILED;
}