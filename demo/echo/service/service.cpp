#include "echo_service_impl.h"
#include "rpc_server.h"

const char *SERVICE_NAME = "echo.EchoService.Echo";  // TODO: register to polaris

int main() {
    RpcServer *server = &RpcServer::GetInst();
    server->Init();

    const std::string path = __FILE__;
    const std::string SERVER_LLOG_CONF_PATH =
        path.substr(0, path.find_last_of("/\\")) + "/../../../config/server_log.cfg";
    server->SetLogConfPath(SERVER_LLOG_CONF_PATH.c_str());
    server->Listen("127.0.0.1", 6688);  // TODO: register to polaris

    EchoServiceImpl echoService;
    RpcServer::AddService(&echoService);

    server->Serve();
    return 0;
}
