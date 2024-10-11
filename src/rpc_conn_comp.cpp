#include "rpc_conn_comp.h"

#include "rpc_macros.h"

RpcConnComp::RpcConnComp()
    : llbc::LLBC_Component(llbc::LLBC_ComponentEvents::DefaultEvents |
                           llbc::LLBC_ComponentEvents::OnUpdate) {}

bool RpcConnComp::OnInit(bool &initFinished) {
    LLOG_TRACE("RpcConnComp OnInit!");
    return true;
}

void RpcConnComp::OnDestroy(bool &destroyFinished) { LLOG_TRACE("Service destroy!"); }

void RpcConnComp::OnSessionCreate(const llbc::LLBC_SessionInfo &sessionInfo) {
    LLOG_TRACE("Session Create: %s", sessionInfo.ToString().c_str());
}

void RpcConnComp::OnSessionDestroy(const llbc::LLBC_SessionDestroyInfo &destroyInfo) {
    LLOG_TRACE("Session Destroy, info: %s", destroyInfo.ToString().c_str());
}

void RpcConnComp::OnAsyncConnResult(const llbc::LLBC_AsyncConnResult &result) {
    LLOG_TRACE("Async-Conn result: %s", result.ToString().c_str());
}

void RpcConnComp::OnUnHandledPacket(const llbc::LLBC_Packet &packet) {
    LLOG_TRACE("Unhandled packet, sessionID: %d, opcode: %d, payloadLen: %ld",
               packet.GetSessionId(), packet.GetOpcode(), packet.GetPayloadLength());
}

void RpcConnComp::OnProtoReport(const llbc::LLBC_ProtoReport &report) {
    LLOG_TRACE("Proto report: %s", report.ToString().c_str());
}

void RpcConnComp::OnUpdate() {
    LLOG_TRACE("OnUpdate");
    llbc::LLBC_Packet *sendPacket =
        llbc::LLBC_GetObjectFromSafetyPool<llbc::LLBC_Packet>();
    if (!sendPacket) {
        LLOG_ERROR("OnUpdate: acquire packet from ojbect pool failed");
        return;
    }
    while (sendQueue_.pop(*sendPacket)) {
        LLOG_TRACE("OnUpdate: sendPacket: %s", sendPacket->ToString().c_str());
        auto ret = GetService()->Send(sendPacket);
        if (ret != LLBC_OK) {
            LLOG_ERROR("Send packet failed, err: %s", llbc::LLBC_FormatLastError());
        }
    }
    LLBC_Recycle(sendPacket);
}

void RpcConnComp::OnRecvPacket(llbc::LLBC_Packet &packet) noexcept {
    LLOG_TRACE("OnRecvPacket: %s", packet.ToString().c_str());
    recvQueue_.emplace(packet);
}

int RpcConnComp::PushSendPacket(llbc::LLBC_Packet &sendPacket) noexcept {
    if (sendQueue_.emplace(sendPacket)) return LLBC_OK;
    return LLBC_FAILED;
}

int RpcConnComp::PopRecvPacket(llbc::LLBC_Packet &recvPacket) noexcept {
    if (recvQueue_.pop(recvPacket)) return LLBC_OK;
    return LLBC_FAILED;
}