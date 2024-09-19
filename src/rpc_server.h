#ifndef _RPC_SERVER_H
#define _RPC_SERVER_H

#include <google/protobuf/service.h>
#include <singleton.h>

class RpcServer : public Singleton<RpcServer> {
    friend class Singleton<RpcServer>;

   public:
    int Init(const char *ip, int port);

    void AddService(::google::protobuf::Service *);

    void Stop() {
        // TODO: implement this function
        stop_ = true;
    }

    void Serve();

   protected:
    RpcServer() = default;

   private:
    bool stop_ = true;
};

#endif  // _RPC_SERVER_H