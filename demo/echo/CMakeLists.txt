set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin/echo")

include_directories(
    ${PB_DIR}
    ${SRC_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/service
    ${CMAKE_CURRENT_LIST_DIR}/client
    ${CMAKE_CURRENT_LIST_DIR}
)

aux_source_directory(
    ${PB_DIR} PB_SRC
)

add_subdirectory(client)
add_subdirectory(service)

