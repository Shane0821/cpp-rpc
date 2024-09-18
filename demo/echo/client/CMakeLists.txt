add_executable(client ${PB_SRC} echo_client_impl.cpp client.cpp)
target_link_libraries(client rpc)