#pragma once

#include "echo.pb.h"

class EchoServiceImpl : public echo::EchoService {
   public:
    void Echo(::google::protobuf::RpcController *controller,
              const ::echo::EchoRequest *request, ::echo::EchoResponse *response,
              ::google::protobuf::Closure *done) override;
    void RelayEcho(::google::protobuf::RpcController *controller,
                   const ::echo::EchoRequest *request, ::echo::EchoResponse *response,
                   ::google::protobuf::Closure *done) override;
};