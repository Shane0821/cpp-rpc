# Define the library
add_library(rpc)

include_directories(${SRC_DIR}/zk)
aux_source_directory(${SRC_DIR}/zk ZK)

aux_source_directory(${SRC_DIR} SRC)
target_sources(rpc PRIVATE ${SRC} ${ZK})

target_link_libraries(rpc libprotobuf.a libllbc.so ${libzk} pthread fmt::fmt uring)