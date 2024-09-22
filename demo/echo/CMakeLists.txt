set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin/echo")


set (CLIENT_DIR ${CMAKE_CURRENT_LIST_DIR}/client)
set (SERVICE_DIR ${CMAKE_CURRENT_LIST_DIR}/service)


include_directories(
    ${PB_DIR}
    ${SRC_DIR}
    ${CLIENT_DIR}
    ${SERVICE_DIR}
    ${CMAKE_CURRENT_LIST_DIR}
)

aux_source_directory(
    ${PB_DIR} PB_SRC
)

file(GLOB CLIENT ${CLIENT_DIR}/echo_service_stub.cpp)

add_subdirectory(client)
add_subdirectory(service)

