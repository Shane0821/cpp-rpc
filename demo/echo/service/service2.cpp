#include "echo_service_impl.h"
#include "rpc_server.h"

const char *SERVER_LLOG_CONF_PATH = "../../config/server_log.cfg";
const char *SERVICE_NAME = "echo.EchoService.Echo";

int main() {
    RpcServer *server = &RpcServer::GetInst();
    server->Init();
    server->SetLogConfPath(SERVER_LLOG_CONF_PATH);
    server->Listen("127.0.0.1", 6699);

    EchoServiceImpl echoService;
    RpcServer::AddService(&echoService);

    server->Serve();
    return 0;
}
