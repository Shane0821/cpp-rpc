#include "echo.pb.h"
#include "echo_service_stub.h"
#include "polaris.h"
#include "rpc_client.h"
#include "rpc_controller.h"

const char *CLIENT_LLOG_CONF_PATH = "../../config/client_log.cfg";
const char *SERVICE_NAME = "echo.EchoService.Echo";

class EchoClient : public RpcClient {
   public:
    virtual RpcCoro CallMethod() override {
        // create rpc req & resp
        echo::EchoRequest req;
        echo::EchoResponse rsp;

        RpcController *cntl = new RpcController();
        cntl->SetCoroHandle(co_await GetHandleAwaiter{});
        RpcChannel *channel =
            RegisterRpcChannel(polaris::NameRegistry["echo.EchoService.Echo"].ip,
                               polaris::NameRegistry["echo.EchoService.Echo"].port);
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
};

int main() {
    EchoClient client;
    client.Init();
    client.SetLogConfPath(CLIENT_LLOG_CONF_PATH);

    LLOG_TRACE("CallMeathod Start");
    client.CallMethod();
    LLOG_TRACE("CallMeathod return");

    return 0;
}
