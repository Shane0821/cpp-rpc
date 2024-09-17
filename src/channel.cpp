#include "channel.h"

#include "conn_mgr.h"
#include "macros.h"

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
    static std::string buffer(MAX_BUFFER_SIZE, '\0');
    ::snprintf(buffer.data(), MAX_BUFFER_SIZE, "src:%u|dst:%u|uid:%lu|seq:%lu|cmd:0x%08X",
               src, dst, uid, seq, cmd);
    return buffer;
}

void RpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor *method,
                            ::google::protobuf::RpcController *controller,
                            const ::google::protobuf::Message *request,
                            ::google::protobuf::Message *response,
                            ::google::protobuf::Closure *done) {
    LLOG_ERROR("NO IMPLEMENTATION|Please use XXServiceStub::Method to start RPC");
}

int RpcChannel::Send(const PkgHead &pkg_head,
                     const ::google::protobuf::Message &message) {
    llbc::LLBC_Packet packet;
    packet.SetHeader(0, RpcOpCode::RpcReq, 0);
    packet.SetSessionId(sessionId_);

    int ret = pkg_head.ToPacket(packet);
    COND_RET_ELOG(ret != 0, ret, "pkg_head.ToPacket failed|ret:%d", ret);
    ret = packet.Write(message);
    COND_RET_ELOG(ret != 0, ret, "packet.Write message failed|ret:%d", ret);
    LLOG_DEBUG("send data|message: %s|packet: %s", message.ShortDebugString().c_str(),
               packet.ToString().c_str());

    connMgr_->PushPacket(packet);
    return 0;
}