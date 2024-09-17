#ifndef _RPC_CONN_MGR_H_
#define _RPC_CONN_MGR_H_

#include <llbc.h>
#include <singleton.h>

#include "conn_comp.h"

class RpcChannel;

class ConnMgr : public Singleton<ConnMgr> {
    friend class Singleton<ConnMgr>;

   public:
    virtual ~ConnMgr();

    int Init() noexcept;
    // start rpc service and listen on ip:port
    int StartRpcService(const char *ip, int port);
    // create rpc client channel
    RpcChannel *CreateRpcChannel(const char *ip, int port);

    int CloseSession(int sessionId);
    int GetServerSessionId() { return server_sessionId_; }

    // subscribe cmdId's data packet
    int Subscribe(int cmdId, const llbc::LLBC_Delegate<void(llbc::LLBC_Packet &)> &deleg);
    // unsubscribe  cmdId's data packet
    void Unsubscribe(int cmdId);

    int PushPacket(llbc::LLBC_Packet &sendPacket) {
        return comp_->PushPacket(sendPacket);
    }
    int PopPacket(llbc::LLBC_Packet &recvPacket) { return comp_->PopPacket(recvPacket); }

    bool IsServer() { return is_server_; }
    // Handle rpc data packets. The main loop should call this function.
    void Tick() noexcept;

   protected:
    ConnMgr() = default;

   private:
    llbc::LLBC_Service *svc_ = nullptr;
    ConnComp *comp_ = nullptr;
    bool is_server_ = false;
    int server_sessionId_ = 0;
    std::unordered_map<int, llbc::LLBC_Delegate<void(llbc::LLBC_Packet &)>>
        packet_delegs_;  // {cmdId : delegate}
};

#endif  // _RPC_CONN_MGR_H_