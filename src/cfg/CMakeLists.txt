find_package(tomlplusplus REQUIRED)

add_library(cfg)

target_sources(
  cfg
  PRIVATE
  config.cpp
  enums.cpp
)

target_include_directories(
  cfg
  PRIVATE
  ${CMAKE_SOURCE_DIR}
)

target_link_libraries(
  cfg
  PUBLIC
  utils
  tomlplusplus::tomlplusplus
)
