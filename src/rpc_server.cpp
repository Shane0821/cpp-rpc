#include "rpc_server.h"

#include <llbc.h>

#include "rpc_conn_mgr.h"
#include "rpc_coro_mgr.h"

void RpcServer::Run() { Serve(); }

void RpcServer::Serve() {
    LLOG_INFO(">>> RPC SERVER START SERVING <<<");

    LLOG_TRACE("START LOOP");

    while (!stop_) {
        RpcCoroMgr::GetInst().HandleCoroTimeout();
        RpcConnMgr::GetInst().Tick();
        llbc::LLBC_Sleep(1);
    }

    LLOG_INFO(">>> RPC SERVER STOP SERVING <<<");
}