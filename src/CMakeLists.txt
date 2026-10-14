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


add_executable(${EXE_NAME})

target_sources(
  ${EXE_NAME}
  PRIVATE
  gojo.cpp
)

add_subdirectory(cfg)
add_subdirectory(package_managers)
add_subdirectory(build_systems)
add_subdirectory(cmd)

target_link_libraries(
  ${EXE_NAME}
  PRIVATE
  build_systems
  cfg
  cmd
  package_managers
  utils
)

target_include_directories(
  ${EXE_NAME}
  PRIVATE
  ${CMAKE_SOURCE_DIR}
)
