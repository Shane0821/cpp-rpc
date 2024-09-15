# Define the library
add_library(librpc)

aux_source_directory(${SRC_DIR} SRC)

# Add source files
target_sources(librpc PRIVATE ${SRC})