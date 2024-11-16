#ifndef _RPC_REGISTRY_H_
#define _RPC_REGISTRY_H_

#include "zk/zk_cpp.h"

class RpcRegistry {
   public:
    struct ServiceAddr {
        const char *ip = nullptr;
        int port = 0;
    };

    static ServiceAddr ParseServiceAddr(const std::string &service_name,
                                        const std::string &path);

    RpcRegistry(const std::string &zk_target);
    ~RpcRegistry() {};

    int RegisterService(const std::string &service_name, const std::string &addr);
    int InitServices(const std::string &service_name);

    ServiceAddr GetRandomService(const std::string &service_name);

   private:
    std::unique_ptr<utility::zk_cpp> client_;
    std::vector<std::string> services;
};

#endif  // _RPC_REGISTRY_H_