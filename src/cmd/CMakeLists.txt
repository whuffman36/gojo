add_library(cmd)

target_sources(
  cmd
  PRIVATE
  build.cpp
  check.cpp
  clean.cpp
  deps.cpp
  fmt.cpp
  help.cpp
  info.cpp
  init.cpp
  profile.cpp
  refresh.cpp
  run.cpp
  test.cpp
)

target_include_directories(
  cmd
  PRIVATE
  ${CMAKE_SOURCE_DIR}
)
