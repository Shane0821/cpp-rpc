#include <llbc.h>
#include <signal.h>

#include <iostream>

#include "rpc_conn_mgr.h"
#include "rpc_macros.h"
#include "rpc_coro_mgr.h"
#include "rpc_server.h"
#include "rpc_service_mgr.h"

#define REGISTER_RPC_CHANNEL(ip, port)                                                   \
    {                                                                                    \
        bool _succ = RpcServiceMgr::GetInst().RegisterChannel(ip, port);                 \
        COND_RET_ELOG(!_succ, EXIT_FAILURE, "register channel failed|ip:%s|port:%d", ip, \
                      port);                                                             \
        LLOG_INFO("register channel succ|ip:%s|port:%d", ip, port);                      \
    }

static const std::string SERVER_LLOG_CONF_PATH =
    "../../config/server_log.cfg";  // change path

void signalHandler(int signum) {
    std::cout << "INTERRUPT SIGNAL [" << signum << "] received.\n";
    RpcServer::GetInst().Stop();
}

int main(int argc, char *argv[]) {
    // 注册信号 SIGINT 和信号处理程序
    signal(SIGINT, signalHandler);

    // 初始化 llbc 库
    llbc::LLBC_Startup();
    // LLBC_HookProcessCrash();
    LLBC_Defer(llbc::LLBC_Cleanup());

    // 初始化日志
    auto ret = LLBC_LoggerMgrSingleton->Initialize(SERVER_LLOG_CONF_PATH);
    if (ret == LLBC_FAILED) {
        // fmt::print("Initialize logger failed|path:{}|error: {}\n",
        // SERVER_LLOG_CONF_PATH,
        //            LLBC_FormatLastError());
        return -1;
    }
    LLOG_TRACE("Hello Server!");

    ret = ConnMgr::GetInst().Init();
    COND_RET_ELOG(ret != 0, ret, "ConnMgr init failed|ret:%d", ret);

    // 启动 rpc 服务
    COND_RET_ELOG(ConnMgr::GetInst().StartRpcService("127.0.0.1", 6688) != LLBC_OK, -1,
                  "ConnMgr StartRpcService failed");

    ret = RpcServiceMgr::GetInst().Init(&ConnMgr::GetInst());
    COND_RET_ELOG(ret != 0, ret, "RpcServiceMgr init failed|ret:%d", ret);
    // bool succ = RpcServiceMgr::GetInst().AddService(nullptr);
    // COND_RET_ELOG(!succ, EXIT_FAILURE, "add service failed");

    REGISTER_RPC_CHANNEL("127.0.0.1", 6688);
    REGISTER_RPC_CHANNEL("127.0.0.1", 6699);

#undef REGISTER_RPC_CHANNEL

    RpcServer::GetInst().Init();
    COND_RET_ELOG(ret != 0, ret, "RpcServer init failed|ret:%d", ret);

    // RpcCoroMgr::GetInst().UseCoro(true);  // 服务端启用协程来处理请求
    RpcServer::GetInst().Run();

    return 0;
}