#ifndef _RPC_CLIENT_H
#define _RPC_CLIENT_H

#include <google/protobuf/service.h>
#include <singleton.h>

#include "rpc_coro.h"

class RpcChannel;

/**
 * To use this class, you must first call Init() to initialize the client. \\
 * Then, you can optionally call SetLogConfPath() to set the path of the log configuration
 * file. \\
 * You should rewrite CallMethod() to call the remote method. \\
 */
class RpcClient {
   public:
    RpcClient() = default;
    ~RpcClient() = default;

    int Init() noexcept;
    void Destroy();

    int SetLogConfPath(const char *log_conf_path);
    RpcChannel *RegisterRpcChannel(const char *ip, int port);

    virtual RpcCoro CallMethod() { co_return; }
    virtual RpcCoro CallMethod(RpcChannel *) { co_return; }

   protected:
    static void SignalHandler(int signum);
    int InitLLBC();
    int InitRpcLib();

    bool initialized_ = false;
};

#endif  // _RPC_CLIENT_H