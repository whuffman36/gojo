add_executable(main_test)

target_sources(
  main_test
  PRIVATE
  # Add source files here
  main_test.cpp
)

target_link_libraries(
  main_test
  PRIVATE
  # Add libraries here
)

target_include_directories(
  main_test
  PRIVATE
  # Add include directories here
  ${CMAKE_SOURCE_DIR}
)

# Add more tests here

add_test(NAME MainTest COMMAND main_test)
# Add more tests here, ex.
# add_test(NAME <test_name> COMMAND <test_target>)
# ...