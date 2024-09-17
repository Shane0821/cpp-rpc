# Define the library
add_library(rpc)

aux_source_directory(${SRC_DIR} SRC)

# Add source files
target_sources(rpc PRIVATE ${SRC})

target_link_libraries(rpc libprotobuf.a libllbc.so)