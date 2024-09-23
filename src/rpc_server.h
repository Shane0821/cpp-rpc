#ifndef _RPC_SERVER_H
#define _RPC_SERVER_H

#include <google/protobuf/service.h>
#include <singleton.h>

#include "rpc_client.h"

class RpcChannel;

/**
 * To use this class, you must first call Init() to initialize the server. \\
 * Then, you can optionally call SetLogConfPath() to set the path of the log configuration
 * file. \\
 * Then, you can call Listen() to start listening on a specific port. \\
 * You can also call AddService() to add  service implementation to the server. \\
 * Finally, you can call Serve() to start serving requests. \\
 */
class RpcServer : public RpcClient, public Singleton<RpcServer> {
    friend class Singleton<RpcServer>;

   public:
    ~RpcServer();

    int Init() noexcept;

    RpcChannel *RegisterRpcChannel(const char *ip, int port);
    int Listen(const char *ip, int port);
    void Stop();
    void Serve();

    static void AddService(::google::protobuf::Service *service);

   protected:
    RpcServer() = default;

    static void SignalHandler(int signum);

    bool stop_ = true;
};

#endif  // _RPC_SERVER_H