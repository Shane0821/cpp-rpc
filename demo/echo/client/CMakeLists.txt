add_executable(client ${PB_SRC} echo_service_stub.cpp client.cpp)
target_link_libraries(client rpc lutil)

add_executable(client2 ${PB_SRC} echo_service_stub.cpp client2.cpp)
target_link_libraries(client2 rpc lutil)