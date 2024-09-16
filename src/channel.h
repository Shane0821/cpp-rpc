#ifndef _RPC_CHANNEL_H
#define _RPC_CHANNEL_H

#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <google/protobuf/stubs/common.h>
#include <llbc.h>
#include <stdlib.h>

class ConnMgr;

class RpcChannel : public ::google::protobuf::RpcChannel {
   public:
    enum RpcOpCode {
        RpcReq = 1,
        RpcRsp = 2,
    };

    // LLBC_Packet:
    //
    //   0                         32                        64
    //   +-------------------------+-------------------------+
    //   |           dst           |           cmd           |
    //   +-------------------------+-------------------------+
    //   |                        uid                        |
    //   +---------------------------------------------------+
    //   |                        seq                        |
    //   +---------------------------------------------------+
    //   |                    body(message)                  |
    //   +---------------------------------------------------+
    //
    struct PkgHead {
        std::uint32_t src = 0U;
        std::uint32_t dst = 0U;
        std::uint64_t uid = 0UL;
        std::uint64_t seq = 0UL;
        std::uint32_t cmd = 0U;
        std::int8_t flag = 0;
        std::int8_t type = 0;

        int FromPacket(llbc::LLBC_Packet &packet);
        int ToPacket(llbc::LLBC_Packet &packet) const;
        const std::string &ToString() const;
    };

    RpcChannel(ConnMgr *connMgr, int sessionId)
        : connMgr_(connMgr), sessionId_(sessionId) {}
    virtual ~RpcChannel();

    void CallMethod(const ::google::protobuf::MethodDescriptor *method,
                    ::google::protobuf::RpcController *controller,
                    const ::google::protobuf::Message *request,
                    ::google::protobuf::Message *response,
                    ::google::protobuf::Closure *done) override;
    int Send(const PkgHead &pkg_head, const ::google::protobuf::Message &message);

    static constexpr std::size_t MAX_BUFFER_SIZE = 1024UL;

   private:
    ConnMgr *connMgr_ = nullptr;
    int sessionId_ = 0;
};

#endif  // _RPC_CHANNEL_H