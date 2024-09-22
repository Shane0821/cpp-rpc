#include "rpc_server.h"

#include <llbc.h>

#include <csignal>

#include "rpc_conn_mgr.h"
#include "rpc_coro_mgr.h"
#include "rpc_service_mgr.h"

RpcServer::~RpcServer() {
    Stop();
    // TODO: cleanup logic
}

void RpcServer::SignalHandler(int signum) {
    std::cout << "SignalHandler: interrupt signal (" << signum << ") received.\n";
    RpcServer::GetInst().Stop();
}

void RpcServer::Init() {
    if (initialized_) {
        return;
    }
    // register signal SIGINT and signal handler
    signal(SIGINT, SignalHandler);

    // init llbc library
    llbc::LLBC_Startup();
    LLBC_Defer(llbc::LLBC_Cleanup());
    initialized_ = true;
}

int RpcServer::SetLogConfPath(const char *log_conf_path) {
    if (!initialized_) {
        std::cout << "RpcServer not initialized.\n";
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

    // init rpc connection manager
    RpcConnMgr *connMgr = &RpcConnMgr::GetInst();
    if (connMgr->Init() != LLBC_OK) {
        LLOG_ERROR("Listen: connMgr Init Fail");
        Stop();
        return LLBC_FAILED;
    }
    // init rpc service manager
    RpcServiceMgr *serviceMgr = &RpcServiceMgr::GetInst();
    if (serviceMgr->Init(connMgr) != LLBC_OK) {
        LLOG_ERROR("Listen: serviceMgr Init Fail");
        Stop();
        return LLBC_FAILED;
    }
    // start rpc connection manager and listen on ip:port
    if (connMgr->StartRpcService(ip, port) != LLBC_OK) {
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
    initialized_ = false;
    stop_ = true;
    LLOG_TRACE("Server stopped.");
    llbc::LLBC_Cleanup();
    std::cout << "RpcServer stopped.\n";
}

void RpcServer::AddService(::google::protobuf::Service *service) {
    if (!RpcServiceMgr::GetInst().AddService(service)) {
        LLOG_ERROR("AddService: add service failed");
    }
}

void RpcServer::Serve() {
    LLOG_INFO(">>> RPC SERVER START SERVING <<<");

    LLOG_TRACE("START LOOP");

    while (!stop_) {
        RpcCoroMgr::GetInst().HandleCoroTimeout();
        RpcConnMgr::GetInst().Tick();
    }

    LLOG_INFO(">>> RPC SERVER STOP SERVING <<<");
}