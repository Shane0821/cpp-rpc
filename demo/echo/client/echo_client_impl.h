#pragma once

#include <coroutine>

#include "echo.pb.h"

class EchoClientImpl {
   public:
    EchoClientImpl(::google::protobuf::RpcChannel *channel) : stub(channel) {};
    ~EchoClientImpl() {};

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