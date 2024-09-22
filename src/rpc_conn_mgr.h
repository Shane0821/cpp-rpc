#ifndef _RPC_CONN_MGR_H_
#define _RPC_CONN_MGR_H_

#include <llbc.h>
#include <singleton.h>

#include "rpc_conn_comp.h"

class RpcChannel;

class RpcConnMgr : public Singleton<RpcConnMgr> {
    friend class Singleton<RpcConnMgr>;

   public:
    virtual ~RpcConnMgr() noexcept;

    int Init() noexcept;

    // start rpc service and listen on ip:port
    int StartRpcService(const char *ip, int port) noexcept;

    // create rpc client channel, this is used to connect to server
    RpcChannel *CreateRpcChannel(const char *ip, int port);

    int CloseSession(int sessionId);

    int GetServerSessionId() { return server_sessionId_; }

    // Subscribe to services' req and rsp packet handlers
    int Subscribe(int cmdId, const llbc::LLBC_Delegate<void(llbc::LLBC_Packet &)> &deleg);
    // Unsubscribe handlers
    void Unsubscribe(int cmdId);

    // add packet to send queue
    int SendPacket(llbc::LLBC_Packet &sendPacket) {
        return comp_->PushSendPacket(sendPacket);
    }
    // get packet from recv queue
    int RecvPacket(llbc::LLBC_Packet &recvPacket) {
        return comp_->PopRecvPacket(recvPacket);
    }

    // Handle rpc data packets. The main loop should call this function.
    void Tick() noexcept;

    bool IsServer() { return is_server_; }

   protected:
    RpcConnMgr() = default;

   private:
    llbc::LLBC_Service *svc_ = nullptr;  // llbc service
    RpcConnComp *comp_ = nullptr;        // rpc conn component
    bool is_server_ = false;             // is server or client
    int server_sessionId_ = 0;           // server session id
    std::unordered_map<int, llbc::LLBC_Delegate<void(llbc::LLBC_Packet &)>>
        packet_delegs_;  // {RpcOpCode : HandleReq / HandleRsp}
};

#endif  // _RPC_CONN_MGR_H_