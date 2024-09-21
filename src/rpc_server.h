#ifndef _RPC_SERVER_H
#define _RPC_SERVER_H

#include <google/protobuf/service.h>
#include <singleton.h>

class RpcServer : public Singleton<RpcServer> {
    friend class Singleton<RpcServer>;

   public:
    ~RpcServer();

    void Init();
    int SetLogConfPath(const char *log_conf_path);
    int Listen(const char *ip, int port);
    void Stop();
    void Serve();

    static void SignalHandler(int signum);
    static void AddService(::google::protobuf::Service *service);

   protected:
    RpcServer() = default;

   private:
    bool stop_ = true;
    bool initialized_ = false;
};

#endif  // _RPC_SERVER_H