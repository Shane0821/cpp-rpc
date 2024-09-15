#ifndef _RPC_SERVER_H
#define _RPC_SERVER_H

#include <singleton.h>

class RpcServer : public Singleton<RpcServer> {
   public:
    void Init() { stop_ = false; }

    void Stop() { stop_ = true; }

    void Run();

   private:
    void Serve();

    bool stop_ = true;
};

#endif  // _RPC_SERVER_H