#include "rpc_registry.h"

#include "rpc_macros.h"

RpcRegistry::RpcRegistry(const std::string &zk_target) {
    client_ = std::make_unique<utility::zk_cpp>();
    client_->connect(zk_target);
}

int RpcRegistry::RegisterService(const std::string &service_name,
                                 const std::string &addr) {
    std::vector<utility::zoo_acl_t> acl;
    acl.push_back(utility::zk_cpp::create_world_acl(utility::zoo_perm_all));
    auto ret = client_->create_ephemeral_node(service_name.c_str(), "", acl);
    COND_RET_ELOG(ret != utility::z_ok, LLBC_FAILED,
                  "RegisterService failed, service_name: %s, addr: %s",
                  service_name.c_str(), addr.c_str());
    COND_RET_ELOG(ret != utility::z_node_exists, LLBC_FAILED,
                  "RegisterService failed, service_name: %s, addr: %s",
                  service_name.c_str(), addr.c_str());

    ret = client_->create_ephemeral_node((service_name + "/" + addr).c_str(), "", acl);
    COND_RET_ELOG(ret == utility::z_node_exists, LLBC_FAILED,
                  "RegisterService failed, addr exsists, service_name: %s, addr: %s",
                  service_name.c_str(), addr.c_str());
    COND_RET_ELOG(ret != utility::z_ok, LLBC_FAILED,
                  "RegisterService failed, service_name: %s, addr: %s",
                  service_name.c_str(), addr.c_str());

    return LLBC_OK;
}

int RpcRegistry::InitServices(const std::string &service_name) {
    std::vector<std::string> children;
    auto ret = client_->get_children(service_name.c_str(), children, false);
    COND_RET_ELOG(ret != utility::z_ok, LLBC_FAILED,
                  "InitServices failed, service_name: %s", service_name.c_str());
    services = children;

    ret = client_->watch_children_event(
        service_name.c_str(),
        [](const std::string &path, const std::vector<std::string> &children) {
            LLOG_INFO("Child_change_events, path[%s] new_child_count[%d]", path.c_str(),
                      (int32_t)children.size());
            for (int i = 0; i < children.size(); i++) {
                LLOG_INFO("Child[%d]: %s", i, children[i]);
            }
        },
        &services);
    COND_RET_ELOG(ret != utility::z_ok, LLBC_FAILED,
                  "InitServices failed, failed to set watcher, service_name: %s",
                  service_name.c_str());

    return LLBC_OK;
}

RpcRegistry::ServiceAddr RpcRegistry::ParseServiceAddr(const std::string &service_name,
                                                       const std::string &path) {
    // remove service_name prefix
    auto tmp = path.substr(service_name.size() + 1);
    // parse ip:port
    auto pos = tmp.find(':');
    if (pos == std::string::npos) {
        return {};
    }
    auto ip = tmp.substr(0, pos);
    auto port = std::stoi(tmp.substr(pos + 1));
    return {ip.c_str(), port};
}

RpcRegistry::ServiceAddr RpcRegistry::GetRandomService(const std::string &service_name) {
    if (services.empty()) {
        return {};
    }
    auto idx = rand() % services.size();
    return ParseServiceAddr(service_name, services[idx]);
}