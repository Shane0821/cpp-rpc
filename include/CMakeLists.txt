add_library(lutil)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} UTIL)

target_sources(lutil PRIVATE ${UTIL})

target_link_libraries(lutil pthread fmt::fmt uring)