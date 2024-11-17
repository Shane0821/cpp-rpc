#ifndef _RPC_CLIENT_H
#define _RPC_CLIENT_H

#include <google/protobuf/service.h>
#include <singleton.h>

#include "rpc_channel.h"
#include "rpc_coro.h"

/**
 * To use this class, you must first call Init() to initialize the client. \\
 * Then, you can optionally call SetLogConfPath() to set the path of the log configuration
 * file. \\
 * You should rewrite CallMethod() to call the remote method. \\
 */
class RpcClient {
   public:
    RpcClient() noexcept = default;
    ~RpcClient() noexcept { Destroy(); }

    int Init() noexcept;
    void Destroy() noexcept;

    int SetLogConfPath(const char *log_conf_path);
    RpcChannel *RegisterRpcChannel(const std::string &);

    void Update();

    /**
     * Call a method using coroutines.
     * You should rewrite this method to call the remote method. Example:
     *
     *  RpcController *cntl = new RpcController(true);
     *  cntl->SetCoroHandle(co_await GetHandleAwaiter{});
     *  EchoServiceStub stub(RegisterRpcChannel(...));
     *  co_await stub.xxx(cntl, &req, &rsp, nullptr);
     *  handle rsp
     *  delete cntl;
     */
    virtual RpcCoro CallMethod() { co_return; }

    /**
     * Blocking version of CallMethod.
     * a should either call CallMethod() or BlockingCallMethod(), not both.
     * you should rewrite this method to call the remote method. Example:
     *
     * RpcController *cntl = new RpcController(false);
     * EchoServiceStub stub(RegisterRpcChannel(...));
     * stub.xxx(cntl, &req, &rsp, nullptr);
     * handle rsp
     * delete cntl;
     */
    virtual void BlockingCallMethod() {}

   protected:
    static void SignalHandler(int signum);
    int InitLLBC();
    int InitRpcLib();

    bool initialized_ = false;
};

#endif  // _RPC_CLIENT_H