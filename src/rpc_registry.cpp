#include "rpc_registry.h"

#include "rpc_macros.h"

int RpcRegistry::Connect(const std::string &url) {
    auto ret = client_->connect(url);
    client_->set_log_lvl(utility::zoo_log_lvl_error);
    COND_RET_ELOG(ret != utility::z_ok, LLBC_FAILED,
                  "RpcRegistry Connect failed, url: %s", url.c_str());
    return LLBC_OK;
}

int RpcRegistry::RegisterService(const std::string &svc_md, const std::string &addr) {
    std::vector<utility::zoo_acl_t> acl;
    acl.push_back(utility::zk_cpp::create_world_acl(utility::zoo_perm_all));
    auto path = "/" + svc_md;
    auto ret = client_->create_persistent_node(path.c_str(), "", acl);
    COND_RET_ELOG(ret != utility::z_node_exists && ret != utility::z_ok, LLBC_FAILED,
                  "RegisterService failed, svc_md: %s, addr: %s, ret: %d", svc_md.c_str(),
                  addr.c_str(), ret);

    ret = client_->create_ephemeral_node((path + "/" + addr).c_str(), "", acl);
    COND_RET_ELOG(ret != utility::z_node_exists && ret != utility::z_ok, LLBC_FAILED,
                  "RegisterService failed, svc_md: %s, addr: %s, ret: %d", svc_md.c_str(),
                  addr.c_str(), ret);

    return LLBC_OK;
}

int RpcRegistry::InitServices(const std::string &svc_md) {
    if (services.find(svc_md) != services.end()) {
        return LLBC_OK;
    }
    std::vector<std::string> children;
    auto path = "/" + svc_md;
    auto ret = client_->get_children(path.c_str(), children, false);
    COND_RET_ELOG(ret != utility::z_ok, LLBC_FAILED, "InitServices failed, svc_md: %s",
                  svc_md.c_str());
    for (int i = 0; i < children.size(); i++) {
        LLOG_INFO("Child[%d]: %s", i, children[i].c_str());
    }
    services[svc_md] = children;

    ret = client_->watch_children_event(
        path.c_str(),
        [](const std::string &path, const std::vector<std::string> &children) {
            LLOG_INFO("Child_change_events, path:%s, new_child_count:%d", path.c_str(),
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
    LLOG_INFO("ParseServiceAddr: svc_md: %s, path: %s", svc_md.c_str(), path.c_str());
    // parse ip:port
    auto pos = path.find(':');
    if (pos == std::string::npos) {
        return {};
    }
    auto ip = path.substr(0, pos);
    auto port = std::stoi(path.substr(pos + 1));
    return {ip, port};
}

RpcRegistry::ServiceAddr RpcRegistry::GetRandomService(const std::string &svc_md) {
    if (InitServices(svc_md) != LLBC_OK) {
        return {};
    }
    if (services[svc_md].empty()) {
        return {};
    }
    LLOG_INFO("GetRandomService: svc_md: %s, service_count: %d", svc_md.c_str(),
              (int32_t)services[svc_md].size());
    auto idx = rand() % services[svc_md].size();
    return ParseServiceAddr(svc_md, services[svc_md][idx]);
}