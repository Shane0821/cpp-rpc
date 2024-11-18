#include "rpc_registry.h"

#include "rpc_macros.h"

int RpcRegistry::Connect(const std::string &url) {
    auto ret = client_->connect(url);
    COND_RET_ELOG(ret != utility::z_ok, LLBC_FAILED,
                  "RpcRegistry Connect failed, url: %s", url.c_str());
    return LLBC_OK;
}

int RpcRegistry::RegisterService(const std::string &svc_md, const std::string &addr) {
    std::vector<utility::zoo_acl_t> acl;
    acl.push_back(utility::zk_cpp::create_world_acl(utility::zoo_perm_all));
    auto path = "/" + svc_md;
    auto ret = client_->create_ephemeral_node(path.c_str(), "", acl);
    COND_RET_ELOG(ret != utility::z_node_exists && ret != utility::z_ok, LLBC_FAILED,
                  "RegisterService failed, svc_md: %s, addr: %s", svc_md.c_str(),
                  addr.c_str());
    ret = client_->create_ephemeral_node((path + "/" + addr).c_str(), "", acl);
    COND_RET_ELOG(ret == utility::z_node_exists, LLBC_FAILED,
                  "RegisterService failed, addr exsists, svc_md: %s, addr: %s",
                  svc_md.c_str(), addr.c_str());
    COND_RET_ELOG(ret != utility::z_ok, LLBC_FAILED,
                  "RegisterService failed, svc_md: %s, addr: %s", svc_md.c_str(),
                  addr.c_str());

    return LLBC_OK;
}

int RpcRegistry::InitServices(const std::string &svc_md) {
    std::vector<std::string> children;
    auto ret = client_->get_children(svc_md.c_str(), children, false);
    COND_RET_ELOG(ret != utility::z_ok, LLBC_FAILED, "InitServices failed, svc_md: %s",
                  svc_md.c_str());
    services[svc_md] = children;

    ret = client_->watch_children_event(
        svc_md.c_str(),
        [](const std::string &path, const std::vector<std::string> &children) {
            LLOG_INFO("Child_change_events, path[%s] new_child_count[%d]", path.c_str(),
                      (int32_t)children.size());
            for (int i = 0; i < children.size(); i++) {
                LLOG_INFO("Child[%d]: %s", i, children[i].c_str());
            }
        },
        &services[svc_md]);
    COND_RET_ELOG(ret != utility::z_ok, LLBC_FAILED,
                  "InitServices failed, failed to set watcher, svc_md: %s",
                  svc_md.c_str());

    return LLBC_OK;
}

RpcRegistry::ServiceAddr RpcRegistry::ParseServiceAddr(const std::string &svc_md,
                                                       const std::string &path) {
    // remove svc_md prefix
    auto tmp = path.substr(svc_md.size() + 1);
    // parse ip:port
    auto pos = tmp.find('.');
    if (pos == std::string::npos) {
        return {};
    }
    auto ip = tmp.substr(0, pos);
    auto port = std::stoi(tmp.substr(pos + 1));
    return {ip.c_str(), port};
}

RpcRegistry::ServiceAddr RpcRegistry::GetRandomService(const std::string &svc_md) {
    if (services.empty()) {
        return {};
    }
    auto idx = rand() % services.size();
    return ParseServiceAddr(svc_md, services[svc_md][idx]);
}