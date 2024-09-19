#include <string>
#include <unordered_map>

struct ServiceAddress {
    const char *ip;
    int port;
};

std::unordered_map<std::string, ServiceAddress> NameRegistry = {
    {"echo.EchoService.Echo", {"127.0.0.1", 6688}},
    {"echo.EchoService.RelayEcho", {"127.0.0.1", 6699}}};