#include "echo_client_impl.h"

std::suspend_always EchoClientImpl::Echo(::google::protobuf::RpcController *controller,
                                         const ::echo::EchoRequest *request,
                                         ::echo::EchoResponse *response,
                                         ::google::protobuf::Closure *done) {
    stub.Echo(controller, request, response, done);
    return {};
}

std::suspend_always EchoClientImpl::RelayEcho(
    ::google::protobuf::RpcController *controller, const ::echo::EchoRequest *request,
    ::echo::EchoResponse *response, ::google::protobuf::Closure *done) {
    stub.RelayEcho(controller, request, response, done);
    return {};
}