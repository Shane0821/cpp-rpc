#include "echo_service_impl.h"

#include <llbc.h>

#include "echo_service_stub.h"
#include "polaris.h"
#include "rpc_channel.h"
#include "rpc_conn_mgr.h"
#include "rpc_controller.h"
#include "rpc_coro.h"

using namespace llbc;

void EchoServiceImpl::Echo(::google::protobuf::RpcController *controller,
                           const ::echo::EchoRequest *request,
                           ::echo::EchoResponse *response,
                           ::google::protobuf::Closure *done) {
    // LLBC_Sleep(10000); timeout test
    LLOG_INFO("received, msg:%s\n", request->msg().c_str());
    response->set_msg(std::string(" Echo >>>>>>> ") + request->msg());
    done->Run();
}

RpcCoro InnerCallEcho(::google::protobuf::RpcController *controller,
                      const ::echo::EchoRequest *req, ::echo::EchoResponse *rsp,
                      ::google::protobuf::Closure *done) {
    // init inner rpc req & rsp
    echo::EchoRequest innerReq;
    innerReq.set_msg("Relay Call >>>>>>" + req->msg());
    LLOG_INFO("call, msg:%s", innerReq.msg().c_str());
    echo::EchoResponse innerRsp;

    // TODO: create channel in advance
    static auto addr = polaris::NameRegistry[EchoServiceImpl::ECHO_SERVICE_NAME];
    RpcChannel *channel = RpcConnMgr::GetInst().CreateRpcChannel(addr.ip, addr.port);
    if (!channel) {
        LLOG_ERROR("CreateRpcChannel for ReplayEcho failed.");
        rsp->set_msg(req->msg() + " ---- inner rpc call server does not exist");
        controller->SetFailed("CreateRpcChannel for ReplayEcho Fail");
        done->Run();
        co_return;
    }

    EchoServiceStub stub(channel);
    RpcController *inner_controller = new RpcController();
    inner_controller->SetCoroHandle(co_await GetHandleAwaiter{});
    co_await stub.Echo(controller, &innerReq, &innerRsp, nullptr);

    LLOG_INFO(
        "InnerCallEcho: recv rsp. status:%s, rsp:%s\n",
        inner_controller->Failed() ? inner_controller->ErrorText().c_str() : "success",
        innerRsp.msg().c_str());

    if (inner_controller->Failed()) {
        controller->SetFailed(inner_controller->ErrorText());
    }

    rsp->set_msg(innerRsp.msg());

    done->Run();
    delete inner_controller;
    co_return;
}

void EchoServiceImpl::RelayEcho(::google::protobuf::RpcController *controller,
                                const ::echo::EchoRequest *req, ::echo::EchoResponse *rsp,
                                ::google::protobuf::Closure *done) {
    LLOG_INFO("received, msg:%s", req->msg().c_str());

    InnerCallEcho(controller, req, rsp, done);
}