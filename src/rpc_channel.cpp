#include "rpc_channel.h"

#include "rpc_conn_mgr.h"
#include "rpc_controller.h"
#include "rpc_coro.h"
#include "rpc_coro_mgr.h"
#include "rpc_macros.h"

RpcChannel::~RpcChannel() { conn_mgr_->CloseSession(session_ID_); }

int RpcChannel::PkgHead::FromPacket(llbc::LLBC_Packet &packet) noexcept {
    int ret = packet.Read(seq);
    COND_RET_ELOG(ret != LLBC_OK, ret, "read pkg_head.seq failed|ret: %d", ret);
    ret = packet.Read(service_name);
    COND_RET_ELOG(ret != LLBC_OK, ret, "read pkg_head.service_name failed|ret: %d", ret);
    ret = packet.Read(method_name);
    COND_RET_ELOG(ret != LLBC_OK, ret, "read pkg_head.method_name failed|ret: %d", ret);

    LLOG_TRACE("read net packet done|%s|session_id: %d", ToString().c_str(),
               packet.GetSessionId());
    return 0;
}

int RpcChannel::PkgHead::ToPacket(llbc::LLBC_Packet &packet) const noexcept {
    int ret = packet.Write(static_cast<std::uint32_t>(seq));
    COND_RET_ELOG(ret != LLBC_OK, ret, "write pkg_head.seq failed|ret: %d", ret);
    ret = packet.Write(service_name);
    COND_RET_ELOG(ret != LLBC_OK, ret, "write pkg_head.service_name failed|ret: %d", ret);
    ret = packet.Write(method_name);
    COND_RET_ELOG(ret != LLBC_OK, ret, "write pkg_head.method_name failed|ret: %d", ret);

    LLOG_TRACE("write net packet done|%s|sessond_id: %d", ToString().c_str(),
               packet.GetSessionId());
    return 0;
}

const std::string &RpcChannel::PkgHead::ToString() const noexcept {
    static std::string buffer;
    std::string().swap(buffer);
    ::snprintf(buffer.data(), MAX_BUFFER_SIZE,
               "seq: %lu|service_name: %s|method_name: %s", seq, service_name.c_str(),
               method_name.c_str());
    return buffer;
}

void RpcChannel::CallMethod(
    const ::google::protobuf::MethodDescriptor *method,
    ::google::protobuf::RpcController *controller,
    const ::google::protobuf::Message *request,
    ::google::protobuf::Message *response,  // handled by coroutine context
    ::google::protobuf::Closure *done) {
    LLOG_TRACE("CallMethod|service: %s|method: %s", method->service()->name().c_str(),
               method->name().c_str());

    RpcController *rpcController = dynamic_cast<RpcController *>(controller);
    COND_RET_ELOG(rpcController == nullptr, ,
                  "CallMethod: controller is not RpcController");

    if (!rpcController->UseCoro()) {
        BlockingCallMethod(method, rpcController, request, response);
        return;
    }

    auto seq = RpcCoroMgr::NewCoroUid();

    // store coroutine context
    RpcCoroMgr::GetInst().AddCoroContext({
        .coro_uid = seq,
        .timeout_time = llbc::LLBC_GetMilliseconds() + RpcCoroMgr::CORO_TIME_OUT,
        .handle = std::coroutine_handle<RpcCoro::promise_type>::from_address(
            rpcController->GetCoroHandle()),
        .rsp = response,
        .controller = rpcController,
    });

    llbc::LLBC_Packet *sendPacket =
        llbc::LLBC_ThreadSpecObjPool::GetSafeObjPool()->Acquire<llbc::LLBC_Packet>();
    COND_RET_ELOG(sendPacket == nullptr,
                  RpcCoroMgr::GetInst().KillCoro(seq, "Acquire LLBC_Packet failed"),
                  "CallMethod: acquire LLBC_Packet failed");

    sendPacket->SetHeader(session_ID_, RpcOpCode::RpcReq, LLBC_OK);

    // set pkg_head
    RpcChannel::PkgHead pkgHead;
    pkgHead.service_name = method->service()->name();
    pkgHead.method_name = method->name();
    pkgHead.seq = seq;

    int ret = pkgHead.ToPacket(*sendPacket);
    COND_RET_ELOG(ret != LLBC_OK,
                  RpcCoroMgr::GetInst().KillCoro(seq, "pkg_head.ToPacket failed"),
                  "CallMethod: pkg_head.ToPacket failed|ret: %d", ret);

    ret = sendPacket->Write(*request);
    COND_RET_ELOG(ret != LLBC_OK,
                  RpcCoroMgr::GetInst().KillCoro(seq, "packet.Write message failed"),
                  "CallMethod: packet.Write message failed|ret: %d", ret);

    LLOG_DEBUG("CallMethod: send data|message: %s|packet: %s",
               request->ShortDebugString().c_str(), sendPacket->ToString().c_str());
    // send packet via conn_mgr
    ret = RpcConnMgr::GetInst().SendPacket(*sendPacket);
    COND_RET_ELOG(ret != LLBC_OK,
                  RpcCoroMgr::GetInst().KillCoro(seq, "sendPacket failed"),
                  "CallMethod: sendPacket failed, ret: %s", llbc::LLBC_FormatLastError());
    LLOG_TRACE("Packet sent. Waiting!");
}

void RpcChannel::BlockingCallMethod(const ::google::protobuf::MethodDescriptor *method,
                                    RpcController *controller,
                                    const ::google::protobuf::Message *request,
                                    ::google::protobuf::Message *response) {
    llbc::LLBC_Packet *sendPacket =
        llbc::LLBC_ThreadSpecObjPool::GetSafeObjPool()->Acquire<llbc::LLBC_Packet>();
    COND_RET_ELOG(sendPacket == nullptr,
                  controller->SetFailed("acquire LLBC_Packet failed"),
                  "CallMethod: acquire LLBC_Packet failed");

    sendPacket->SetHeader(session_ID_, RpcOpCode::RpcReq, LLBC_OK);

    // set pkg_head
    RpcChannel::PkgHead pkgHead;
    pkgHead.service_name = method->service()->name();
    pkgHead.method_name = method->name();
    pkgHead.seq = 0;

    int ret = pkgHead.ToPacket(*sendPacket);
    COND_RET_ELOG(ret != LLBC_OK, controller->SetFailed("pkg_head.ToPacket failed"),
                  "CallMethod: pkg_head.ToPacket failed|ret: %d", ret);

    ret = sendPacket->Write(*request);
    COND_RET_ELOG(ret != LLBC_OK, controller->SetFailed("packet.Write message failed"),
                  "CallMethod: packet.Write message failed|ret: %d", ret);

    LLOG_DEBUG("CallMethod: send data|message: %s|packet: %s",
               request->ShortDebugString().c_str(), sendPacket->ToString().c_str());
    // send packet via conn_mgr
    ret = RpcConnMgr::GetInst().SendPacket(*sendPacket);
    COND_RET_ELOG(ret != LLBC_OK, controller->SetFailed("sendPacket failed"),
                  "CallMethod: sendPacket failed, ret: %s", llbc::LLBC_FormatLastError());

    LLOG_TRACE("Packet sent. Waiting!");

    auto recvPacket = sendPacket;
    int count = 0;
    while (!conn_mgr_->RecvPacket(*recvPacket) && count < RpcCoroMgr::CORO_TIME_OUT) {
        llbc::LLBC_Sleep(1);
        ++count;
    }

    if (!recvPacket) {
        response->Clear();
        LLOG_ERROR("BlockingCallMethod: receive packet timeout!");
        controller->SetFailed("receive packet timeout");
    }

    LLOG_TRACE("BlockingCallMethod: payload info|length:%lu|info: %s",
               recvPacket->GetPayloadLength(), recvPacket->ToString().c_str());

    PkgHead pkg_head;
    auto ret = pkg_head.FromPacket(*recvPacket);
    COND_RET_ELOG(ret != LLBC_OK, , "BlockingCallMethod: parse net packet failed|ret:%d",
                  ret);
    ret = recvPacket->Read(*response);
    COND_RET_ELOG(ret != LLBC_OK, , "BlockingCallMethod: read recv_packet failed|ret:%d",
                  ret);

    LLOG_TRACE("BlockingCallMethod: recved: %s|extdata:%lu",
               response->ShortDebugString().c_str(), pkg_head.seq);
    LLBC_Recycle(sendPacket);
}