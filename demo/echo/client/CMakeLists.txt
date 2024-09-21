add_executable(client ${PB_SRC} echo_service_stub.cpp client.cpp)
target_link_libraries(client rpc)