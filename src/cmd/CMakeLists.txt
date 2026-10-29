add_library(cmd)

target_sources(
  cmd
  PRIVATE
  build.cpp
  check.cpp
  clean.cpp
  dep.cpp
  enums.cpp
  fmt.cpp
  help.cpp
  info.cpp
  init.cpp
  new.cpp
  profile.cpp
  refresh.cpp
  run.cpp
  test.cpp
  update.cpp
)

target_include_directories(
  cmd
  PRIVATE
  ${CMAKE_SOURCE_DIR}
)

target_link_libraries(
  cmd
  PRIVATE
  build_systems
  cfg
  package_managers
  utils
)
