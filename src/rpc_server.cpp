#include "rpc_server.h"

#include <llbc.h>

#include "rpc_conn_mgr.h"
#include "rpc_coro_mgr.h"
#include "rpc_service_mgr.h"

int RpcServer::Init(const char *ip, int port) {
    if (!stop_) {
        LLOG_ERROR("RpcServer already started");
        return LLBC_FAILED;
    }
    stop_ = false;
    RpcConnMgr *connMgr = &RpcConnMgr::GetInst();
    connMgr->Init();
    RpcServiceMgr *serviceMgr = &RpcServiceMgr::GetInst();
    serviceMgr->Init(connMgr);
    if (connMgr->StartRpcService(ip, port) != LLBC_OK) {
        LLOG_ERROR("connMgr StartRpcService Fail");
        return LLBC_FAILED;
    }
    return LLBC_OK;
}

void RpcServer::AddService(::google::protobuf::Service *service) {
    RpcServiceMgr *serviceMgr = &RpcServiceMgr::GetInst();
    serviceMgr->AddService(service);
}

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