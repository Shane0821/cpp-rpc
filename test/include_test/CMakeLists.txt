aux_source_directory(
  ${CMAKE_CURRENT_SOURCE_DIR} SRC
)

add_executable(
  include_test
  ${SRC}
)

target_link_libraries(
  include_test
  GTest::gtest_main
)

gtest_discover_tests(include_test)