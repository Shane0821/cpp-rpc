#ifndef _RPC_CONN_MGR_H_
#define _RPC_CONN_MGR_H_

#include <llbc.h>
#include <singleton.h>

#include "conn_comp.h"

class RpcChannel;

class ConnMgr : public Singleton<ConnMgr> {
   public:
    ConnMgr() {};
    virtual ~ConnMgr();

    int Init();
    int StartRpcService(const char *ip, int port);
    // create rpc client channel
    RpcChannel *CreateRpcChannel(const char *ip, int port);

    int CloseSession(int sessionId);
    int GetServerSessionId() { return serverSessionId_; }

    // subscribe cmdId's data packet
    int Subscribe(int cmdId, const llbc::LLBC_Delegate<void(llbc::LLBC_Packet &)> &deleg);
    // unsubscribe  cmdId's data packet
    void Unsubscribe(int cmdId);

    int PushPacket(llbc::LLBC_Packet &sendPacket) {
        return comp_->PushPacket(sendPacket);
    }
    int PopPacket(llbc::LLBC_Packet &recvPacket) { return comp_->PopPacket(recvPacket); }

    bool IsServer() { return isServer_; }
    // Handle rpc data packets. The main loop should call this function.
    bool Tick();

   private:
    llbc::LLBC_Service *svc_ = nullptr;
    ConnComp *comp_ = nullptr;
    bool isServer_ = false;
    int serverSessionId_ = 0;
    std::unordered_map<int, llbc::LLBC_Delegate<void(llbc::LLBC_Packet &)>>
        packetDelegs_;  // {cmdId : delegate}
};

#endif  // _RPC_CONN_MGR_H_