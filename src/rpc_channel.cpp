#include "rpc_channel.h"

#include "rpc_conn_mgr.h"
#include "rpc_controller.h"
#include "rpc_macros.h"

RpcChannel::~RpcChannel() { connMgr_->CloseSession(sessionId_); }

int RpcChannel::PkgHead::FromPacket(llbc::LLBC_Packet &packet) {
    int ret = packet.Read(dst);
    COND_RET_ELOG(ret != LLBC_OK, ret, "read pkg_head.dst failed|ret:%d", ret);
    ret = packet.Read(cmd);
    COND_RET_ELOG(ret != LLBC_OK, ret, "read pkg_head.cmd failed|ret:%d", ret);
    ret = packet.Read(uid);
    COND_RET_ELOG(ret != LLBC_OK, ret, "read pkg_head.uid failed|ret:%d", ret);
    ret = packet.Read(seq);
    COND_RET_ELOG(ret != LLBC_OK, ret, "read pkg_head.seq failed|ret:%d", ret);

    LLOG_TRACE("read net packet done|%s|session_id:%d", ToString().c_str(),
               packet.GetSessionId());
    return 0;
}

int RpcChannel::PkgHead::ToPacket(llbc::LLBC_Packet &packet) const {
    int ret = packet.Write(static_cast<std::uint32_t>(dst));
    COND_RET_ELOG(ret != LLBC_OK, ret, "write pkg_head.dst failed|ret:%d", ret);
    ret = packet.Write(static_cast<std::uint32_t>(cmd));
    COND_RET_ELOG(ret != LLBC_OK, ret, "write pkg_head.cmd failed|ret:%d", ret);
    ret = packet.Write(static_cast<std::uint64_t>(uid));
    COND_RET_ELOG(ret != LLBC_OK, ret, "write pkg_head.uid failed|ret:%d", ret);
    ret = packet.Write(static_cast<std::uint64_t>(seq));
    COND_RET_ELOG(ret != LLBC_OK, ret, "write pkg_head.seq failed|ret:%d", ret);

    LLOG_TRACE("write net packet done|%s|sessond_id:%d", ToString().c_str(),
               packet.GetSessionId());
    return 0;
}

const std::string &RpcChannel::PkgHead::ToString() const {
    static std::string buffer;
    std::string().swap(buffer);
    ::snprintf(buffer.data(), MAX_BUFFER_SIZE, "src:%u|dst:%u|uid:%lu|seq:%lu|cmd:0x%08X",
               src, dst, uid, seq, cmd);
    return buffer;
}

void RpcChannel::CallMethod(
    const ::google::protobuf::MethodDescriptor *method,
    ::google::protobuf::RpcController *controller,
    const ::google::protobuf::Message *request,
    ::google::protobuf::Message *response,  // handled by coroutine context
    ::google::protobuf::Closure *done) {
    LLOG_TRACE("CallMethod!");

    RpcController *rpcController = dynamic_cast<RpcController *>(controller);
    COND_RET_ELOG(rpcController == nullptr, , "controller is not RpcController");

    response->Clear();

    llbc::LLBC_Packet *sendPacket =
        llbc::LLBC_ThreadSpecObjPool::GetSafeObjPool()->Acquire<llbc::LLBC_Packet>();
    sendPacket->SetHeader(0, RpcOpCode::RpcReq, 0);
    sendPacket->SetSessionId(sessionId_);

    int ret = rpcController->GetPkgHead().ToPacket(*sendPacket);
    COND_RET_ELOG(ret != LLBC_OK, , "pkg_head.ToPacket failed|ret:%d", ret);
    ret = sendPacket->Write(*request);
    COND_RET_ELOG(ret != LLBC_OK, , "packet.Write message failed|ret:%d", ret);
    LLOG_DEBUG("send data|message: %s|packet: %s", request->ShortDebugString().c_str(),
               sendPacket->ToString().c_str());

    // send packet via conn_mgr
    ret = RpcConnMgr::GetInst().SendPacket(*sendPacket);
    COND_RET_ELOG(ret != LLBC_OK, , "PushPacket failed, ret: %s",
                  llbc::LLBC_FormatLastError());

    LLOG_TRACE("Waiting!");
}