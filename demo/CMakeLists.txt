set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

include_directories(
    ${PB_DIR}
)

aux_source_directory(
    ${PB_DIR} PB_SRC
)

add_subdirectory(service)
add_subdirectory(client)

