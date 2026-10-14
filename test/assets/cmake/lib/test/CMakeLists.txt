add_executable(lib_test)

target_sources(
  lib_test
  PRIVATE
  # Add source files here
  lib_test.cpp
)

target_link_libraries(
  lib_test
  PRIVATE
  # Add libraries here
)

target_include_directories(
  lib_test
  PRIVATE
  # Add include directories here
  ${CMAKE_SOURCE_DIR}
)

# Add more tests here

add_test(NAME LibTest COMMAND lib_test)
# Add more tests here, ex.
# add_test(NAME <test_name> COMMAND <test_target>)
# ...