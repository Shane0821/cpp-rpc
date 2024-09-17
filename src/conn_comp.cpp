#include "conn_comp.h"

#include "macros.h"

ConnComp::ConnComp() : llbc::LLBC_Component() {}

bool ConnComp::OnInit(bool &initFinished) {
    LLOG_TRACE("Service create!");
    return true;
}

void ConnComp::OnDestroy(bool &destroyFinished) { LLOG_TRACE("Service destroy!"); }

void ConnComp::OnSessionCreate(const llbc::LLBC_SessionInfo &sessionInfo) {
    LLOG_TRACE("Session Create: %s", sessionInfo.ToString().c_str());
}

void ConnComp::OnSessionDestroy(const llbc::LLBC_SessionDestroyInfo &destroyInfo) {
    LLOG_TRACE("Session Destroy, info: %s", destroyInfo.ToString().c_str());
}

void ConnComp::OnAsyncConnResult(const llbc::LLBC_AsyncConnResult &result) {
    LLOG_TRACE("Async-Conn result: %s", result.ToString().c_str());
}

void ConnComp::OnUnHandledPacket(const llbc::LLBC_Packet &packet) {
    LLOG_TRACE("Unhandled packet, sessionId: %d, opcode: %d, payloadLen: %ld",
               packet.GetSessionId(), packet.GetOpcode(), packet.GetPayloadLength());
}

void ConnComp::OnProtoReport(const llbc::LLBC_ProtoReport &report) {
    LLOG_TRACE("Proto report: %s", report.ToString().c_str());
}

void ConnComp::OnUpdate() {
    static llbc::LLBC_Packet sendPacket;
    while (sendQueue_.pop(sendPacket)) {
        LLOG_TRACE("sendPacket: %s", sendPacket.ToString().c_str());
        auto ret = GetService()->Send(&sendPacket);
        if (ret != LLBC_OK) {
            LLOG_ERROR("Send packet failed, err: %s", llbc::LLBC_FormatLastError());
        }
    }
}

void ConnComp::OnRecvPacket(llbc::LLBC_Packet &packet) noexcept {
    LLOG_TRACE("OnRecvPacket: %s", packet.ToString().c_str());
    static llbc::LLBC_Packet recvPacket;
    recvPacket.SetHeader(packet.GetSessionId(), packet.GetOpcode(), 0);
    recvPacket.SetPayload(packet.DetachPayload());
    recvQueue_.emplace(recvPacket);
}

int ConnComp::PushPacket(llbc::LLBC_Packet &sendPacket) noexcept {
    if (sendQueue_.emplace(sendPacket)) return LLBC_OK;
    return LLBC_FAILED;
}

int ConnComp::PopPacket(llbc::LLBC_Packet &recvPacket) noexcept {
    if (recvQueue_.pop(recvPacket)) return LLBC_OK;
    return LLBC_FAILED;
}