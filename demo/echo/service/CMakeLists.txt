add_executable(server ${PB_SRC} ${CLIENT} echo_service_impl.cpp service.cpp)
target_link_libraries(server rpc lutil)

add_executable(server2 ${PB_SRC} ${CLIENT} echo_service_impl.cpp service2.cpp)
target_link_libraries(server2 rpc lutil)