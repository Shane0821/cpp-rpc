#include <string>
#include <unordered_map>

std::unordered_map<std::string, std::pair<const char *, int>> NameRegistry = {
    {"echo.EchoService.Echo", {"127.0.0.1", 6688}},
    {"echo.EchoService.RelayEcho", {"127.0.0.1", 6699}}
};