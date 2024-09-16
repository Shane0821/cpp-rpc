#include "conn_mgr.h"

#include "channel.h"
#include "macros.h"

ConnMgr::~ConnMgr() {
    if (svc_) {
        svc_->Stop();
        delete svc_;
    }
    delete comp_;
}

int ConnMgr::Init() {
    // Create service
    svc_ = llbc::LLBC_Service::Create("Svc");
    comp_ = new ConnComp;
    svc_->AddComponent(comp_);
    svc_->Subscribe(RpcChannel::RpcOpCode::RpcReq, comp_, &ConnComp::OnRecvPacket);
    svc_->Subscribe(RpcChannel::RpcOpCode::RpcRsp, comp_, &ConnComp::OnRecvPacket);
    svc_->SuppressCoderNotFoundWarning();
    auto ret = svc_->Start(1);
    LLOG_TRACE("Service start, ret: %d", ret);
    return ret;
}

int ConnMgr::StartRpcService(const char *ip, int port) {
    LLOG_TRACE("ConnMgr StartRpcService");
    LLOG_TRACE("Server will listen on %s:%d", ip, port);
    int serverSessionId_ = svc_->Listen(ip, port);
    COND_RET_ELOG(serverSessionId_ == 0, LLBC_FAILED, "Create session failed, reason: %s",
                  llbc::LLBC_FormatLastError())

    isServer_ = true;
    return LLBC_OK;
}

RpcChannel *ConnMgr::CreateRpcChannel(const char *ip, int port) {
    LLOG_TRACE("CreateRpcChannel");
    auto sessionId = svc_->Connect(ip, port);
    COND_RET_ELOG(sessionId == 0, nullptr, "Create session failed, reason: %s",
                  llbc::LLBC_FormatLastError());

    return new RpcChannel(this, sessionId);
}

int ConnMgr::CloseSession(int sessionId) {
    LLOG_TRACE("CloseSession, %d", sessionId);
    return svc_->RemoveSession(sessionId);
}

bool ConnMgr::Tick() {
    auto ret = false;
    static llbc::LLBC_Packet packet;
    while (PopPacket(packet) == LLBC_OK) {
        ret = true;
        LLOG_TRACE("Tick");
        auto it = packetDelegs_.find(packet.GetOpcode());
        if (it == packetDelegs_.end())
            LLOG_ERROR("Recv Untapped opcode:%d", packet.GetOpcode());
        else
            (it->second)(packet);
    }

    return ret;
}

int ConnMgr::Subscribe(int cmdId,
                       const llbc::LLBC_Delegate<void(llbc::LLBC_Packet &)> &deleg) {
    auto pair = packetDelegs_.emplace(cmdId, deleg);
    COND_RET(!pair.second, LLBC_FAILED);
    return LLBC_OK;
}

void ConnMgr::Unsubscribe(int cmdId) { packetDelegs_.erase(cmdId); }