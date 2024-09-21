#pragma once

#include <coroutine>

#include "echo.pb.h"

class EchoServiceStub {
   public:
    EchoServiceStub(::google::protobuf::RpcChannel *channel) : stub(channel) {};
    ~EchoServiceStub() {};

    std::suspend_always Echo(::google::protobuf::RpcController *controller,
                             const ::echo::EchoRequest *request,
                             ::echo::EchoResponse *response,
                             ::google::protobuf::Closure *done);
    std::suspend_always RelayEcho(::google::protobuf::RpcController *controller,
                                  const ::echo::EchoRequest *request,
                                  ::echo::EchoResponse *response,
                                  ::google::protobuf::Closure *done);

   private:
    ::echo::EchoService_Stub stub;
};