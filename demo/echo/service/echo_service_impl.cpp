#include "echo_service_impl.h"

#include <llbc.h>

#include "rpc_channel.h"
#include "rpc_conn_mgr.h"
#include "rpc_coro.h"
#include "echo_client_impl.h"

using namespace llbc;

void EchoServiceImpl::Echo(::google::protobuf::RpcController *controller,
                           const ::echo::EchoRequest *request,
                           ::echo::EchoResponse *response,
                           ::google::protobuf::Closure *done) {
    // LLBC_Sleep(5000); timeout test
    LLOG_INFO("received, msg:%s\n", request->msg().c_str());
    response->set_msg(std::string(" Echo >>>>>>> ") + request->msg());
    done->Run();
}

RpcCoro InnerCallMeathod(::google::protobuf::RpcController *controller,
                         const ::echo::EchoRequest *req, ::echo::EchoResponse *rsp,
                         ::google::protobuf::Closure *done) {
    LLOG_INFO("received, msg:%s", req->msg().c_str());
    // init inner rpc req & rsp
    echo::EchoRequest innerReq;
    innerReq.set_msg("Relay Call >>>>>>" + req->msg());
    echo::EchoResponse innerRsp;
    // create rpc channel
    RpcChannel *channel = RpcConnMgr::GetInst().CreateRpcChannel("127.0.0.1", 6699);
    if (!channel) {
        LLOG_INFO("GetRpcChannel Fail");
        rsp->set_msg(req->msg() + " ---- inner rpc call server not exist");
        controller->SetFailed("GetRpcChannel Fail");
        done->Run();
        co_return;
    }

    LLOG_INFO("call, msg:%s", innerReq.msg().c_str());

    EchoServiceStub stub(channel);
    // // RpcController cntl(co_await GetHandleAwaiter{});
    // // co_await stub.Echo(&cntl, &innerReq, &innerRsp, nullptr);
    // // LLOG_INFO("Recv rsp, status:%s, rsp:%s\n",
    // //           cntl.Failed() ? cntl.ErrorText().c_str() : "success",
    // //           innerRsp.msg().c_str());
    // // if (cntl.Failed()) {
    // //     controller->SetFailed(cntl.ErrorText());
    // // }

    rsp->set_msg(innerRsp.msg());
    done->Run();

    co_return;
}

void EchoServiceImpl::RelayEcho(::google::protobuf::RpcController *controller,
                                const ::echo::EchoRequest *req, ::echo::EchoResponse *rsp,
                                ::google::protobuf::Closure *done) {
    LLOG_TRACE("RECEIVED MSG: %s", req->msg().c_str());

    InnerCallMeathod(controller, req, rsp, done);
}