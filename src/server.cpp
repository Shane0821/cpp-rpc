#include <llbc.h>

#include "conn_mgr.h"
#include "server.h"

void RpcServer::Run() {
    Serve();
}

void RpcServer::Serve() {
    LLOG_INFO(">>> RPC SERVER START SERVING <<<");

    LLOG_TRACE("START LOOP");

    while (!stop_) {
        ConnMgr::GetInst().Tick();
        llbc::LLBC_Sleep(1);
    }

    LLOG_INFO(">>> RPC SERVER STOP SERVING <<<");
}