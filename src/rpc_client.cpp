#include "rpc_client.h"

#include <llbc.h>

#include <csignal>

#include "rpc_conn_mgr.h"
#include "rpc_service_mgr.h"

void RpcClient::SignalHandler(int signum) {
    std::cout << "RpcClient SignalHandler: interrupt signal (" << signum
              << ") received.\n";
    std::terminate();
}

int RpcClient::SetLogConfPath(const char *log_conf_path) {
    if (!initialized_) {
        std::cout << "SetLogConfPath: RpcClient not initialized.\n";
        return LLBC_FAILED;
    }
    auto ret = LLBC_LoggerMgrSingleton->Initialize(log_conf_path);
    if (ret == LLBC_FAILED) {
        LLOG_ERROR("SetLogConfPath: initialize logger failed|path: %s|error: %s",
                   log_conf_path, llbc::LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    return LLBC_OK;
}

int RpcClient::Init() {
    if (initialized_) {
        std::cout << "Init: RpcClient is already initialized.\n";
        return LLBC_FAILED;
    }

    // register signal SIGINT and signal handler
    signal(SIGINT, SignalHandler);

    // init llbc library
    llbc::LLBC_Startup();
    LLBC_Defer(llbc::LLBC_Cleanup());

    // init rpc connection manager
    RpcConnMgr *connMgr = &RpcConnMgr::GetInst();
    if (connMgr->Init() != LLBC_OK) {
        LLOG_ERROR("Init: connMgr Init Fail");
        Destroy();
        return LLBC_FAILED;
    }

    // init rpc service manager
    RpcServiceMgr *serviceMgr = &RpcServiceMgr::GetInst();
    if (serviceMgr->Init(connMgr) != LLBC_OK) {
        LLOG_ERROR("Init: serviceMgr Init Fail");
        Destroy();
        return LLBC_FAILED;
    }

    initialized_ = true;
    return LLBC_OK;
}

void RpcClient::Destroy() {
    if (!initialized_) {
        std::cout << "RpcClient not initialized.\n";
        return;
    }
    llbc::LLBC_Cleanup();
    initialized_ = false;
    std::cout << "RpcClient uninitialized.\n";
}

RpcChannel *RpcClient::RegisterRpcChannel(const char *ip, int port) {
    if (!initialized_) {
        std::cout << "RpcClient not initialized.\n";
        return nullptr;
    }
    return RpcServiceMgr::GetInst().RegisterRpcChannel(ip, port);
}