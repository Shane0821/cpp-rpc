#ifndef _RPC_CHANNEL_H
#define _RPC_CHANNEL_H

#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <google/protobuf/stubs/common.h>
#include <llbc.h>
#include <stdlib.h>

class RpcConnMgr;

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
    //   |                        uid                        |
    //   +---------------------------------------------------+
    //   |                        seq                        |
    //   +---------------------------------------------------+
    //   |                    service_name                   |
    //   +---------------------------------------------------+
    //   |                    method_name                    |
    //   +---------------------------------------------------+
    //   |                    body(message)                  |
    //   +---------------------------------------------------+
    //
    struct PkgHead {
        std::uint64_t uid = 0UL;  // usr id
        std::uint64_t seq = 0UL;  // coro_uid
        std::string service_name;
        std::string method_name;

        int FromPacket(llbc::LLBC_Packet &packet) noexcept;
        int ToPacket(llbc::LLBC_Packet &packet) const noexcept;
        const std::string &ToString() const noexcept;
    };

    RpcChannel(RpcConnMgr *connMgr, int sessionId)
        : connMgr_(connMgr), sessionId_(sessionId) {}
    virtual ~RpcChannel();

    virtual void CallMethod(const ::google::protobuf::MethodDescriptor *method,
                            ::google::protobuf::RpcController *controller,
                            const ::google::protobuf::Message *request,
                            ::google::protobuf::Message *response,
                            ::google::protobuf::Closure *done) override;

    static constexpr std::size_t MAX_BUFFER_SIZE = 1024UL;

   private:
    RpcConnMgr *connMgr_ = nullptr;
    int sessionId_ = 0;
};

#endif  // _RPC_CHANNEL_H