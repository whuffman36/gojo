add_library(utils)

target_sources(
  utils
  PRIVATE
  utils.cpp
)

target_include_directories(
  utils
  PRIVATE
  ${CMAKE_SOURCE_DIR}
)


add_executable(gojo)

target_sources(
  gojo
  PRIVATE
  gojo.cpp
)

add_subdirectory(cfg)
add_subdirectory(package_managers)
add_subdirectory(build_systems)
add_subdirectory(cmd)

target_link_libraries(
  gojo
  PRIVATE
  build_systems
  cfg
  cmd
  package_managers
  utils
)

target_include_directories(
  gojo
  PRIVATE
  ${CMAKE_SOURCE_DIR}
)
