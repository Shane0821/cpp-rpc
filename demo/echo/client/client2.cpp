#include <memory>

#include "echo.pb.h"
#include "echo_service_stub.h"
#include "rpc_client.h"
#include "rpc_controller.h"

class EchoClient : public RpcClient {
   public:
    virtual RpcCoro CallMethod() override {
        // create rpc req & resp
        echo::EchoRequest req;
        echo::EchoResponse rsp;

        RpcChannel *channel = RegisterRpcChannel("EchoService.RelayEcho");
        if (!channel) {
            co_return;
        }

        std::unique_ptr<RpcController> cntl = std::make_unique<RpcController>(true);

        cntl->SetCoroHandle(co_await GetHandleAwaiter{});

        EchoServiceStub stub(channel);

        req.set_msg("Hello, Echo.");
        LLOG_INFO("EchoClient rpc echo call: msg:%s", req.msg().c_str());
        co_await stub.RelayEcho(cntl.get(), &req, &rsp, nullptr);
        LLOG_INFO("Recv Echo Rsp, status:%s, rsp:%s",
                  cntl->Failed() ? cntl->ErrorText().c_str() : "success",
                  rsp.msg().c_str());

        co_return;
    }

    virtual void BlockingCallMethod() override {
        // create rpc req & resp
        echo::EchoRequest req;
        echo::EchoResponse rsp;

        RpcChannel *channel = RegisterRpcChannel("EchoService.RelayEcho");
        if (!channel) {
            return;
        }

        std::unique_ptr<RpcController> cntl = std::make_unique<RpcController>(false);
        EchoServiceStub stub(channel);

        req.set_msg("Hello, Echo.");
        LLOG_INFO("EchoClient rpc echo call: msg:%s", req.msg().c_str());
        stub.RelayEcho(cntl.get(), &req, &rsp, nullptr);
        LLOG_INFO("Recv Echo Rsp, status:%s, rsp:%s",
                  cntl->Failed() ? cntl->ErrorText().c_str() : "success",
                  rsp.msg().c_str());
    }
};

int main() {
    EchoClient client;
    client.Init();

    const std::string path = __FILE__;
    const std::string CLIENT_LLOG_CONF_PATH =
        path.substr(0, path.find_last_of("/\\")) + "/../../../config/client_log.cfg";
    client.SetLogConfPath(CLIENT_LLOG_CONF_PATH.c_str());

    // LLOG_TRACE("CallMethod Start");
    // client.CallMethod();
    // LLOG_TRACE("CallMethod return");
    // for (int i = 0; i < 20000; i++) {
    //     client.Update();
    // }

    LLOG_TRACE("BlockingCallMethod Start");
    client.BlockingCallMethod();
    LLOG_TRACE("BlockingCallMethod return");
    return 0;
}
