#include "echo_service_stub.h"

std::suspend_always EchoServiceStub::Echo(::google::protobuf::RpcController *controller,
                                          const ::echo::EchoRequest *request,
                                          ::echo::EchoResponse *response,
                                          ::google::protobuf::Closure *done) {
    stub.Echo(controller, request, response, done);
    return {};
}

std::suspend_always EchoServiceStub::RelayEcho(
    ::google::protobuf::RpcController *controller, const ::echo::EchoRequest *request,
    ::echo::EchoResponse *response, ::google::protobuf::Closure *done) {
    stub.RelayEcho(controller, request, response, done);
    return {};
}