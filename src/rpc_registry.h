#ifndef _RPC_REGISTRY_H_
#define _RPC_REGISTRY_H_

#include "zk/zk_cpp.h"  // TODO: check

class RpcRegistry {
   public:
    struct ServiceAddr {
        const char *ip = nullptr;
        int port = 0;
    };

    static ServiceAddr ParseServiceAddr(const std::string &svc_md,
                                        const std::string &path);

    RpcRegistry() : client_(std::make_unique<utility::zk_cpp>()) {};
    ~RpcRegistry() {};

    int Connect(const std::string &url);

    int RegisterService(const std::string &svc_md, const std::string &addr);
    int InitServices(const std::string &svc_md);

    ServiceAddr GetRandomService(const std::string &svc_md);

   private:
    std::unique_ptr<utility::zk_cpp> client_;
    std::unordered_map<std::string, std::vector<std::string>>
        services;  // svc_md -> service_addrs
};

#endif  // _RPC_REGISTRY_H_