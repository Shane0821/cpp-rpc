#include "rpc_server.h"

#include <llbc.h>

#include <csignal>

#include "rpc_conn_mgr.h"
#include "rpc_coro_mgr.h"
#include "rpc_service_mgr.h"

RpcServer::~RpcServer() { Stop(); }

void RpcServer::SignalHandler(int signum) {
    std::cout << "RpcServer SignalHandler: interrupt signal (" << signum
              << ") received.\n";
    RpcServer::GetInst().Stop();
}

int RpcServer::Init() noexcept {
    RpcClient::Init();

    // register signal SIGINT and signal handler
    signal(SIGINT, SignalHandler);

    return LLBC_OK;
}

RpcChannel *RpcServer::RegisterRpcChannel(const char *ip, int port) {
    if (stop_) {
        std::cout << "RpcServer not started.\n";
        return nullptr;
    }
    return RpcServiceMgr::GetInst().RegisterRpcChannel(ip, port);
}

int RpcServer::Listen(const char *ip, int port) {
    if (!initialized_) {
        std::cout << "RpcServer not initialized.\n";
        return LLBC_FAILED;
    }
    if (!stop_) {
        std::cout << "RpcServer is already listening.\n";
        return LLBC_FAILED;
    }
    LLOG_TRACE("Hello Server!");
    stop_ = false;

    // start rpc connection manager and listen on ip:port
    if (RpcConnMgr::GetInst().StartRpcService(ip, port) != LLBC_OK) {
        LLOG_ERROR("Listen: connMgr StartRpcService Fail");
        Stop();
        return LLBC_FAILED;
    }
    return LLBC_OK;
}

void RpcServer::Stop() {
    if (stop_) {
        std::cout << "RpcServer already stopped.\n";
        return;
    }
    stop_ = true;
    LLOG_TRACE("Server Stop Set.");
}

void RpcServer::AddService(::google::protobuf::Service *service) {
    if (!RpcServiceMgr::GetInst().AddService(service)) {
        LLOG_ERROR("AddService: add service failed");
    }
}

void RpcServer::Serve() {
    if (stop_) {
        std::cout << "RpcServer not started.\n";
        return;
    }

    LLOG_INFO(">>> RPC SERVER START SERVING <<<");

    LLOG_TRACE("START LOOP");

    while (!stop_) {
        RpcCoroMgr::GetInst().HandleCoroTimeout();
        RpcConnMgr::GetInst().Tick();
        llbc::LLBC_Sleep(1);
    }

    LLOG_INFO(">>> RPC SERVER STOP SERVING <<<");

    RpcClient::Destroy();
}