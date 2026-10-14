find_package(GTest REQUIRED)

# Link all following targets to GoogleTest.
link_libraries(gtest::gtest)

add_executable(example_test)

target_sources(
  example_test
  PRIVATE
  # Add source files here
  example_test.cpp
)

target_link_libraries(
  example_test
  PRIVATE
  # Add libraries here
  
)

target_include_directories(
  example_test
  PRIVATE
  # Add include directories here
  "${CMAKE_SOURCE_DIR}"
)

# Add more tests here

include(GoogleTest)
gtest_discover_tests(example_test)
# Add more tests here, ex.
# gtest_discover_tests(<test_target>)
# ...

