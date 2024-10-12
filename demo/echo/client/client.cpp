#include "echo.pb.h"
#include "echo_service_stub.h"
#include "polaris.h"
#include "rpc_client.h"
#include "rpc_controller.h"

const char *SERVICE_NAME = "echo.EchoService.Echo";

class EchoClient : public RpcClient {
   public:
    virtual RpcCoro CallMethod() override {
        // create rpc req & resp
        echo::EchoRequest req;
        echo::EchoResponse rsp;

        RpcChannel *channel =
            RegisterRpcChannel(polaris::NameRegistry["echo.EchoService.Echo"].ip,
                               polaris::NameRegistry["echo.EchoService.Echo"].port);

        if (!channel) {
            co_return;
        }

        RpcController *cntl = new RpcController(true);
        cntl->SetCoroHandle(co_await GetHandleAwaiter{});

        EchoServiceStub stub(channel);

        req.set_msg("Hello, Echo.");
        LLOG_INFO("EchoClient rpc echo call: msg:%s", req.msg().c_str());
        co_await stub.Echo(cntl, &req, &rsp, nullptr);
        LLOG_INFO("Recv Echo Rsp, status:%s, rsp:%s",
                  cntl->Failed() ? cntl->ErrorText().c_str() : "success",
                  rsp.msg().c_str());

        delete cntl;
        co_return;
    }

    virtual void BlockingCallMethod() override {
        // create rpc req & resp
        echo::EchoRequest req;
        echo::EchoResponse rsp;

        RpcChannel *channel =
            RegisterRpcChannel(polaris::NameRegistry["echo.EchoService.Echo"].ip,
                               polaris::NameRegistry["echo.EchoService.Echo"].port);
        if (!channel) {
            return;
        }

        RpcController *cntl = new RpcController(false);
        EchoServiceStub stub(channel);

        req.set_msg("Hello, Echo.");
        LLOG_INFO("EchoClient rpc echo call: msg:%s", req.msg().c_str());
        stub.Echo(cntl, &req, &rsp, nullptr);
        LLOG_INFO("Recv Echo Rsp, status:%s, rsp:%s",
                  cntl->Failed() ? cntl->ErrorText().c_str() : "success",
                  rsp.msg().c_str());

        delete cntl;
    }
};

int main() {
    EchoClient client;
    client.Init();

    const std::string path = __FILE__;
    const std::string CLIENT_LLOG_CONF_PATH =
        path.substr(0, path.find_last_of("/\\")) + "/../../../config/client_log.cfg";
    client.SetLogConfPath(CLIENT_LLOG_CONF_PATH.c_str());

    LLOG_TRACE("CallMeathod Start");
    client.CallMethod();
    LLOG_TRACE("CallMeathod return");
    sleep(20);

    // LLOG_TRACE("BlockingCallMethod Start");
    // client.BlockingCallMethod();
    // LLOG_TRACE("BlockingCallMethod return");
    return 0;
}
