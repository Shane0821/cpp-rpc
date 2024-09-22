#include <llbc.h>
#include <signal.h>

#include <iostream>

#include "echo.pb.h"
#include "echo_service_stub.h"
#include "rpc_channel.h"
#include "rpc_conn_mgr.h"
#include "rpc_coro_mgr.h"
#include "rpc_macros.h"
#include "rpc_service_mgr.h"

bool stop = false;
static const std::string CLIENT_LLOG_CONF_PATH =
    "../../config/client_log.cfg";  // change path

void signalHandler(int signum) {
    std::cout << "INTERRUPT SIGNAL [" << signum << "] received.\n";
    stop = true;
}

void mainloop() {
    // 创建 rpc req & resp
    echo::EchoRequest req;
    echo::EchoResponse rsp;

    while (!stop) {
        std::string input;
        COND_EXP(!(std::cin >> input), break);  // 手动阻塞
        COND_EXP(input == "exit", break);
        // req.set_msg(input);
        // auto ret = echo::   ::DemoServiceStub::Echo(0UL, req, &rsp);
        // COND_EXP_ELOG(ret != 0, continue, "call Stub::method failed|ret:%d", ret);
    }
}

int main(int argc, char *argv[]) {
    // 注册信号 SIGINT 和信号处理程序
    ::signal(SIGINT, signalHandler);

    // 初始化 llbc 库
    llbc::LLBC_Startup();
    LLBC_Defer(llbc::LLBC_Cleanup());

    // 初始化日志
    auto ret = LLBC_LoggerMgrSingleton->Initialize(CLIENT_LLOG_CONF_PATH);
    if (ret == LLBC_FAILED) {
        // fmt::print("Initialize logger failed|path:{}|error: {}\n",
        // CLIENT_LLOG_CONF_PATH,
        //            llbc::LLBC_FormatLastError());
        return EXIT_FAILURE;
    }

    // 初始化连接管理器
    ret = RpcConnMgr::GetInst().Init();
    COND_RET_ELOG(ret != LLBC_OK, -1, "init RpcConnMgr failed|error: %s",
                  llbc::LLBC_FormatLastError());

    // RpcCoroMgr::GetInst().UseCoro(false);  // 客户端默认不用协程，一直阻塞等待回包

    // 协程方案, 在新协程中 call rpc
    ret = RpcServiceMgr::GetInst().Init(&RpcConnMgr::GetInst());
    COND_RET_ELOG(ret != 0, ret, "RpcServiceMgr init failed|ret:%d", ret);
    // RpcServiceMgr::GetInst().RegisterChannel("127.0.0.1", 6688);

    mainloop();

    LLOG_TRACE("client stop");

    return 0;
}