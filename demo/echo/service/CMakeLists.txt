add_executable(server ${PB_SRC} ${CLIENT} echo_service_impl.cpp service.cpp)
target_link_libraries(server rpc)