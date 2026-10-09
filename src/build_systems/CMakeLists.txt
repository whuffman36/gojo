add_library(build_systems)

target_sources(
  build_systems
  PRIVATE
  cmake.cpp
)

target_include_directories(
  build_systems
  PRIVATE
  ${CMAKE_SOURCE_DIR}
)

target_link_libraries(
  build_systems
  PRIVATE
  cfg
  utils
)
