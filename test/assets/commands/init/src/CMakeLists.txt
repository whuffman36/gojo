add_executable(example)

target_sources(
  example
  PRIVATE
  # Add source files here
  example.cpp
)

target_link_libraries(
  example
  PRIVATE
  # Add libraries here
)

target_include_directories(
  example
  PRIVATE
  # Add include directories here
  "${CMAKE_SOURCE_DIR}"
)

# Add other targets here, ex.
# add_library(<lib_name> STATIC)
#
# target_sources(
#   <lib_name>
#   PRIVATE
#   lib.cpp
#   ...
# )
# ...
